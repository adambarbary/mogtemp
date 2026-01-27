// Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
// based on: http://swingleydev.org/blog/tag/arduino/ (no longer available)
// Interface design, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8
// Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials
// Save icons created by Yogi Aprelliyanto - Flaticon https://www.flaticon.com/free-icons/save
// Return icons created by Kiranshastry - Flaticon https://www.flaticon.com/free-icons/return

// FLOW V1 
// MX Temp and difference alarm
// 4 portal sensor display
// Adjustable brightness. 
// Adjustable difference setting
// Adjustable maximum temp

// Included Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ui.h"
#include "screens.h"
#include <lvgl.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <bb_spi_lcd.h>

extern objects_t objects;

BB_SPI_LCD lcd;

// Pin definitions for 2.8" ILI9341 CYD
#define TOUCH_MISO 39
#define TOUCH_MOSI 32
#define TOUCH_CLK 25
#define TOUCH_CS 33
#define TFT_BL 21  // Backlight pin for CYD

// Screen resolution
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define FONT_SIZE 2

// LVGL buffer size
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint8_t *draw_buf;

// Touchscreen variables
uint16_t x, y, z;
uint32_t lastTick = 0;

// Temperature sensor definitions
#define ONE_WIRE_BUS 27
#define TEMPERATURE_PRECISION 12
#define alarm 22 
#define TEMP_READ_INTERVAL 4000  // ms between temperature readings
#define UI_UPDATE_DELAY 5        // ms between UI updates
#define ALARM_OFF LOW
#define ALARM_ON HIGH

// Configure Temperature sensors
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const DeviceAddress thermOne   = { 0x28,0xE1,0xB9,0x78,0x70,0x23,0x6,0xAE };
const DeviceAddress thermTwo   = { 0x28,0x8D,0x30,0x44,0x70,0x23,0x6,0x72 };
const DeviceAddress thermThree = { 0x28,0xE8,0x7D,0x5C,0x70,0x23,0x6,0x7B };
const DeviceAddress thermFour  = { 0x28,0x56,0xDC,0x98,0x70,0x23,0x6,0xA6 };

// Initial settings state
bool alarmOn = true;
int maxtemp = 33; // Initial max temperature before alarm
int maxdiff = 4; // Initial max difference between left & right portals
int currentBrightness = 255; // Default full brightness

//________________________________________________________________________________
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

//________________________________________________________________________________
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  TOUCHINFO ti;
  if (lcd.rtReadTouch(&ti)) {
    y = ti.y[0];
    x = 240 - ti.x[0];
    z = ti.pressure[0];

    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;

  }
  
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

//________________________________________________________________________________
// Function to set screen brightness
void setScreenBrightness(int brightness) {
  // Brightness should be 0-255
  brightness = constrain(brightness, 0, 255);
  analogWrite(TFT_BL, brightness);
  currentBrightness = brightness;
  
 // Serial.print("Screen brightness set to: ");
 // Serial.println(brightness);
}

//________________________________________________________________________________
// Slider callback - controls screen brightness
static void my_horizontal_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
  int32_t value = lv_slider_get_value(slider);
  
  // Map slider value to brightness (adjust range as needed)
  // If your slider goes 0-100, map to 0-255
  int brightness = map(value, 0, 100, 0, 255);
  setScreenBrightness(brightness);
}

// Settings Page
//________________________________________________________________________________
// Slider callback - controls Maxtemp
static void max_temp_slider_value_changed_event_cb(lv_event_t * e) {
    lv_obj_t * max_temp_slider = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t * max_temp = (lv_obj_t *)lv_event_get_user_data(e);
    
    maxtemp = (int)lv_slider_get_value(max_temp_slider);
    
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", maxtemp);  // Use the variable
    lv_label_set_text(max_temp, buf);
}

// Slider callback - controls Maxdiff
static void max_diff_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * max_diff_slider = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t * max_diff = (lv_obj_t *)lv_event_get_user_data(e);
  
  maxdiff = (int)lv_slider_get_value(max_diff_slider);

  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", maxdiff);  // Use the variable
  lv_label_set_text(max_diff, buf);
}


// Callback that is triggered when "alarm_switch" is clicked/toggled.
static void alarm_switch_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * alarm_switch = (lv_obj_t*) lv_event_get_target(e);
    lv_obj_t * alarm_switch_label = (lv_obj_t *)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        bool checked = lv_obj_has_state(alarm_switch, LV_STATE_CHECKED);
        
        if (checked) {
            lv_label_set_text(alarm_switch_label, "ON");
            lv_obj_set_style_text_color(alarm_switch_label, lv_color_hex(0xffff7f50), LV_PART_MAIN | LV_STATE_DEFAULT);
            alarmOn = true;
        } else {
            lv_label_set_text(alarm_switch_label, "OFF");
            lv_obj_set_style_text_color(alarm_switch_label, lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
            alarmOn = false;
        }
    }
}
//________________________________________________________________________________ 

//________________________________________________________________________________
void printTemperature(DeviceAddress deviceAddress) {
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print(tempC);
}

//________________________________________________________________________________
void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Mog Portal Dash");
    Serial.println();

    String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + 
                          "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);
    
    // Setup alarm pin
    pinMode(alarm, OUTPUT);
    digitalWrite(alarm, ALARM_OFF);
    
    // Setup backlight pin for brightness control
    pinMode(TFT_BL, OUTPUT);
    setScreenBrightness(255); // Start at full brightness

    //---------------------------------------- LVGL setup
    Serial.println();
    Serial.println("Start LVGL Setup.");

    // Initialize Display using BB_SPI_LCD
    lcd.begin(DISPLAY_CYD_2USB);
    lcd.setRotation(0);
    lcd.setFont(FONT_12x16);

    // Start LVGL
    lv_init();
    lv_log_register_print_cb(log_print);

    // Create display object
    lv_display_t * disp;
    draw_buf = new uint8_t[DRAW_BUF_SIZE];
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);

    // Initialize Touch
    lcd.rtInit(TOUCH_MOSI, TOUCH_MISO, TOUCH_CLK, TOUCH_CS);

    // Initialize LVGL input device (Touchscreen)
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchscreen_read);

    Serial.println();
    Serial.println("LVGL Setup Completed.");

    // Integrate EEZ Studio GUI
    ui_init();

    // Set initial slider values to match variables
    lv_slider_set_value(objects.max_temp_slider, maxtemp, LV_ANIM_OFF);
    lv_slider_set_value(objects.max_diff_slider, maxdiff, LV_ANIM_OFF);
    
    // Set initial alarm switch state
    if (alarmOn) {
        lv_obj_add_state(objects.alarm_switch, LV_STATE_CHECKED);
    }

    // Register slider event handler for brightness control
    lv_obj_add_event_cb(objects.my_horizontal_slider, 
                        my_horizontal_slider_value_changed_event_cb, 
                        LV_EVENT_VALUE_CHANGED, 
                        NULL);

    //settings events
    // Register slider event handler for MaxTemp control
    lv_obj_add_event_cb(objects.max_temp_slider, 
                    max_temp_slider_value_changed_event_cb, 
                    LV_EVENT_VALUE_CHANGED, 
                    objects.max_temp);

    // Register slider event handler for MaxDiff control
    lv_obj_add_event_cb(objects.max_diff_slider, 
                    max_diff_slider_value_changed_event_cb, 
                    LV_EVENT_VALUE_CHANGED, 
                    objects.max_diff);               

    
    // Register "alarm_switch" event handler.
    lv_obj_add_event_cb(objects.alarm_switch, alarm_switch_event_handler, LV_EVENT_VALUE_CHANGED, objects.alarm_switch_label);
    
    // Activate Sensors
    sensors.begin();
    sensors.setResolution(thermOne, TEMPERATURE_PRECISION);
    sensors.setResolution(thermTwo, TEMPERATURE_PRECISION);
    sensors.setResolution(thermThree, TEMPERATURE_PRECISION);
    sensors.setResolution(thermFour, TEMPERATURE_PRECISION);
    
    Serial.println("Setup complete - watching for interactions...");
}

//________________________________________________________________________________
void loop() {
    static unsigned long lastTempRead = 0;
    
    lv_tick_inc(millis() - lastTick);
    lastTick = millis();
    lv_timer_handler();
    eez_flow_tick();
    
    // Only read temperatures periodically
    if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
        lastTempRead = millis();
        
        sensors.requestTemperatures();
        int thermOneC = (int)sensors.getTempC(thermOne);
        int thermTwoC = (int)sensors.getTempC(thermTwo);
        int thermThreeC = (int)sensors.getTempC(thermThree);
        int thermFourC = (int)sensors.getTempC(thermFour);
    
        int difffront = abs(thermOneC - thermTwoC);
        int diffrear = abs(thermThreeC - thermFourC);
        int diff = max(difffront, diffrear);
    
        int maxfront = max(thermOneC, thermTwoC);
        int maxrear = max(thermThreeC, thermFourC);
        int maxall = max(maxfront, maxrear);

        // Temperature alarm
        if (alarmOn) {
            bool tempTooHigh = (maxall > maxtemp);
            bool diffTooLarge = (diff > maxdiff);
    
            if (tempTooHigh || diffTooLarge) {
                digitalWrite(alarm, ALARM_ON);
            } else {
                digitalWrite(alarm, ALARM_OFF);
            }
        } else {
            digitalWrite(alarm, ALARM_OFF);
        }   
    
        // Update TFT display arcs - MOVED INSIDE THE IF BLOCK
        char buf[5];
        
        // Front left Portal
        lv_snprintf(buf, sizeof(buf), "%d", thermOneC);
        lv_arc_set_value(objects.fl_portal_guage, thermOneC);
        lv_label_set_text(objects.fl_portal_label, buf);
        
        // Front right Portal
        lv_snprintf(buf, sizeof(buf), "%d", thermTwoC);
        lv_arc_set_value(objects.fr_portal_guage, thermTwoC);
        lv_label_set_text(objects.fr_portal_label, buf);
        
        // Rear left Portal
        lv_snprintf(buf, sizeof(buf), "%d", thermThreeC);
        lv_arc_set_value(objects.rl_portal_guage, thermThreeC);
        lv_label_set_text(objects.rl_portal_label, buf);

        // Rear right Portal
        lv_snprintf(buf, sizeof(buf), "%d", thermFourC);
        lv_arc_set_value(objects.rr_portal_guage, thermFourC);
        lv_label_set_text(objects.rr_portal_label, buf);
    }  // Close the if block here
    
    delay(UI_UPDATE_DELAY);
}