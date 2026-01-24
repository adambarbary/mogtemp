// Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
// based on: http://swingleydev.org/blog/tag/arduino/ (no longer available)
// Interface design, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8
// Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials

// Included Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ui.h"
#include <lvgl.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <bb_spi_lcd.h>

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
#define maxdiff 4  // Max difference between left & right portals
#define maxtemp 33 // Max temperature before alarm

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress thermOne   = { 0x28,0xE1,0xB9,0x78,0x70,0x23,0x6,0xAE };
DeviceAddress thermTwo   = { 0x28,0x8D,0x30,0x44,0x70,0x23,0x6,0x72 };
DeviceAddress thermThree = { 0x28,0xE8,0x7D,0x5C,0x70,0x23,0x6,0x7B };
DeviceAddress thermFour  = { 0x28,0x56,0xDC,0x98,0x70,0x23,0x6,0xA6 };

// Settings state
bool settingsMenuOpen = false;
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

   /**Serial.print("Touch: X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" Z=");
    Serial.println(z);
    */
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
  
 // Serial.print("Slider value changed to: ");
 // Serial.println(value);
  
  // Map slider value to brightness (adjust range as needed)
  // If your slider goes 0-100, map to 0-255
  int brightness = map(value, 0, 100, 0, 255);
  setScreenBrightness(brightness);
}

//________________________________________________________________________________
// Settings button callback
static void settings_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  
  //Serial.print("Settings event received, code: ");
 // Serial.println(code);
  
 /* if (code == LV_EVENT_CLICKED) {
    settingsMenuOpen = !settingsMenuOpen;
    
    Serial.println("========================================");
    Serial.println("*** SETTINGS BUTTON CLICKED! ***");
    Serial.print("Menu is now: ");
    Serial.println(settingsMenuOpen ? "OPEN" : "CLOSED");
    Serial.println("========================================");
    
    // Add your settings menu logic here
    // For example, show/hide a settings panel:
    // if (settingsMenuOpen) {
    //   lv_obj_clear_flag(objects.settings_panel, LV_OBJ_FLAG_HIDDEN);
    // } else {
    //   lv_obj_add_flag(objects.settings_panel, LV_OBJ_FLAG_HIDDEN);
    // }
  } else if (code == LV_EVENT_PRESSED) {
    Serial.println("Settings button PRESSED");
  } else if (code == LV_EVENT_RELEASED) {
    Serial.println("Settings button RELEASED");
  } */
}

//________________________________________________________________________________
void printTemperature(DeviceAddress deviceAddress) {
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print(tempC);
}

//________________________________________________________________________________
void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(3000);

    Serial.println("Mog Portal Dash");
    Serial.println();
    delay(500);

    String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + 
                          "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);
    delay(500);
    
    // Setup alarm pin
    pinMode(alarm, OUTPUT);
    digitalWrite(alarm, LOW);
    
    // Setup backlight pin for brightness control
    pinMode(TFT_BL, OUTPUT);
    setScreenBrightness(255); // Start at full brightness

    //---------------------------------------- LVGL setup
    Serial.println();
    Serial.println("Start LVGL Setup.");
    delay(500);

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
    delay(500);

    // Integrate EEZ Studio GUI
    ui_init();

    // Register slider event handler for brightness control
    lv_obj_add_event_cb(objects.my_horizontal_slider, 
                        my_horizontal_slider_value_changed_event_cb, 
                        LV_EVENT_VALUE_CHANGED, 
                        NULL);

    // Register settings button - use ALL events to debug
    lv_obj_add_event_cb(objects.settings, 
                        settings_event_handler, 
                        LV_EVENT_ALL, 
                        NULL);

    // Make absolutely sure the settings object is clickable
    lv_obj_add_flag(objects.settings, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(objects.settings, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_clear_flag(objects.settings, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(objects.settings, LV_OBJ_FLAG_SCROLL_CHAIN);
    
    // Verify settings object exists and log it
    if (objects.settings != NULL) {
        Serial.println("Settings object initialized successfully");
        Serial.print("Settings object flags: ");
        Serial.println(lv_obj_get_state(objects.settings));
    } else {
        Serial.println("ERROR: Settings object is NULL!");
    }

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
    lv_tick_inc(millis() - lastTick);
    lastTick = millis();
    lv_timer_handler(); // Process LVGL tasks - this is critical for events!
    delay(5);
 
    // Request temperatures from all sensors
    sensors.requestTemperatures();
    
  /*  // Serial monitor output
    printTemperature(thermOne);
    Serial.print("-fl,");
    printTemperature(thermTwo);
    Serial.print("-fr,");
    printTemperature(thermThree);
    Serial.print("-rl,");
    printTemperature(thermFour);
    Serial.print("-rr,");
   */
    // Read sensors and calculate alarm status
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
    
    Serial.print(difffront);
    Serial.print("front,");
    Serial.print(diffrear);
    Serial.print("rear,");
   
    // Temperature alarm
    if (maxall > maxtemp) {
        Serial.println("max temp high");
        digitalWrite(alarm, HIGH);
    } else {
        Serial.println("max temp off");
        digitalWrite(alarm, LOW);
    }
    
    // Update TFT display arcs
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

    delay(1000); // Refresh rate
}