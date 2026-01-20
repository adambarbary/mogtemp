// Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
// based on: http://swingleydev.org/blog/tag/arduino/ (no longer available)
// Interface design, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8 - https://drive.google.com/file/d/1s4dtXSGeFvR-TereeB9-4iUszsPacVj_/view
// Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/

// Included Libraries
#include <OneWire.h> // For enabling one wire temp probes
#include <DallasTemperature.h> // To convert one wire probe values to temperatures
#include "ui.h"
#include <lvgl.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

TFT_eSPI tft = TFT_eSPI();

// Touchscreen PINs
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

// Declaring the "XPT2046_Touchscreen" object as "touchscreen" and its settings.
SPIClass touchscreenSPI = SPIClass(HSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// Defines the screen resolution.
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2

//---------------------------------------- Defines the Touchscreen calibration result value.
#define touchscreen_Min_X 245
#define touchscreen_Max_X 3847
#define touchscreen_Min_Y 184
#define touchscreen_Max_Y 3833
//---------------------------------------- 

// LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes.
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint8_t *draw_buf;

// Variables for x, y and z values ​​on the touchscreen.
uint16_t x, y, z;

// Used to track the tick timer.
uint32_t lastTick = 0;

int Count_Val = 0;
//-----------------------------------------

// Definte Temp sensors
#define ONE_WIRE_BUS 27 // one wire data is on pin 8
#define TEMPERATURE_PRECISION 12
#define alarm 22 //alarm on pin 13 - ie the internal LED also acts as an indicator
//#define alarm2 21 //alarm on pin 11 
#define maxdiff 4 //the maximum difference allowed between right & left portals on any axle before alarm is triggered. Change this value as required.
#define maxtemp 33//the maximum temperature allowed on any portal before alarm is triggered. Change this value as required

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress thermOne = { 0x28,0xE1,0xB9,0x78,0x70,0x23,0x6,0xAE };//sets up the individual sensor address - need to identify what each sensors' address is before anything
DeviceAddress thermTwo = { 0x28,0x8D,0x30,0x44,0x70,0x23,0x6,0x72 };
DeviceAddress thermThree = { 0x28,0xE8,0x7D,0x5C,0x70,0x23,0x6,0x7B };
DeviceAddress thermFour = { 0x28,0x56,0xDC,0x98,0x70,0x23,0x6,0xA6 };

//LiquidCrystal lcd(2, 3, 7, 6, 5, 4);//pins for LCD

//________________________________________________________________________________ log_print()
// If logging is enabled, it will inform the user about what is happening in the library.
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}
//________________________________________________________________________________ 



//________________________________________________________________________________ touchscreen_read()
// Get the Touchscreen data.
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if (touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    
    //---------------------------------------- 
    // Calibrate Touchscreen points with map function to the correct width and height.
    x = map(p.x, touchscreen_Max_X, touchscreen_Min_X, 1, SCREEN_HEIGHT);
    y = map(p.y, touchscreen_Max_Y, touchscreen_Min_Y, 1, SCREEN_WIDTH);
    
    // If the touchscreen on the TFT LCD is upside down, try the code line below.
    // If there is an error on the touchscreen, edit and try other settings in the code section below.
    // Because my TFT LCD Touchscreen device may have different settings from your TFT LCD device.
    //x = map(p.x, touchscreen_Min_X, touchscreen_Max_X, 1, SCREEN_HEIGHT);
    //y = map(p.y, touchscreen_Min_Y, touchscreen_Max_Y, 1, SCREEN_WIDTH);
    //---------------------------------------- 
    
    z = p.z;

    // Set the coordinates.
    data->point.x = x;
    data->point.y = y;

    data->state = LV_INDEV_STATE_PRESSED;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor.
    Serial.print("X = ");
    Serial.print(x);
    Serial.print(" | Y = ");
    Serial.print(y);
    Serial.print(" | Pressure = ");
    Serial.print(z);
    Serial.println();
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}
//________________________________________________________________________________ 

// Callback to print Slider value (my_horizontal_slider) to Label (my_label_horizontal_slider)
// to be displayed on TFT LCD and change Canvas color.
// This callback is triggered when Slider value changes.
static void my_horizontal_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * my_horizontal_slider = (lv_obj_t *)lv_event_get_target(e);            //--> Slider that generated the event.
  //lv_obj_t * my_label_horizontal_slider = (lv_obj_t *)lv_event_get_user_data(e);   //--> Label or other UI elements we want to update (Optional).
  
  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", (int)lv_slider_get_value(my_horizontal_slider));
  //lv_label_set_text(my_label_horizontal_slider, buf);
}
//________________________________________________________________________________ 



//________________________________________________________________________________ my_image_esp32_event_handler()

//Callback that is triggered when "settings" is clicked. Modify this for the settings later. 
static void settings_event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  
  if (code == LV_EVENT_CLICKED) {
   // lv_label_set_text(objects.my_label_img_sta, "ESP32 image is\nTouched.");
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(3000);

    Serial.println("Mog Portal Dash");
    Serial.println();
    delay(500);

    String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);
    delay(500);
    
    pinMode(alarm, OUTPUT);
    digitalWrite(alarm, LOW);
    //pinMode(alarm2, OUTPUT);
    //digitalWrite(alarm2, LOW);

    //---------------------------------------- LVGL setup.
    Serial.println();
    Serial.println("Start LVGL Setup.");
    delay(500);

    // Start the SPI for the touchscreen and init the touchscreen.
    //touchscreen.begin();
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    // Set the Touchscreen rotation in landscape mode
    // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
    touchscreen.setRotation(0);

    // Start LVGL.
    lv_init();
  
    // Register print function for debugging.
    lv_log_register_print_cb(log_print);

    // Create a display object.
    lv_display_t * disp;
  
    // Initialize the TFT display using the TFT_eSPI library.
    draw_buf = new uint8_t[DRAW_BUF_SIZE];
    //disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
    // If the display on the TFT LCD has problems, try the line of code below:
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
    
    // Initialize an LVGL input device object (Touchscreen).
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  
    // Set the callback function to read Touchscreen input.
    lv_indev_set_read_cb(indev, touchscreen_read);

    Serial.println();
    Serial.println("LVGL Setup Completed.");
    delay(500);
    //---------------------------------------- 

    // Integrate EEZ Studio GUI.
    ui_init();

    // Register "my_horizontal_slider" event handler.
    //lv_obj_add_event_cb(objects.my_horizontal_slider, my_horizontal_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, objects.my_label_horizontal_slider);
    lv_obj_add_event_cb(objects.my_horizontal_slider, my_horizontal_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Register "settings" event handler.
    lv_obj_add_event_cb(objects.settings, settings_event_handler, LV_EVENT_ALL, NULL);

  
    // Activate Sensors
    sensors.begin();
    sensors.setResolution(thermOne, TEMPERATURE_PRECISION);
    sensors.setResolution(thermTwo, TEMPERATURE_PRECISION);
    sensors.setResolution(thermThree, TEMPERATURE_PRECISION);
    sensors.setResolution(thermFour, TEMPERATURE_PRECISION);
}

void printTemperature(DeviceAddress deviceAddress) {
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print(tempC);
}


void loop() {
    lv_tick_inc(millis() - lastTick); //--> Update the tick timer. Tick is new for LVGL 9.
    lastTick = millis();
    lv_timer_handler(); //--> Update the UI.
    delay(5);
 
    // serial monitor output of temperatures
    sensors.requestTemperatures();
    printTemperature(thermOne);
    Serial.print("-fl,");
    printTemperature(thermTwo);
    Serial.print("-fr,");
    printTemperature(thermThree);
    Serial.print("-rl,");
    printTemperature(thermFour);
    Serial.print("-rr,");
   
    // Read sensors and calculate alarm status
    int thermOneC =(sensors.getTempC(thermOne));
    int thermTwoC =(sensors.getTempC(thermTwo));
    int thermThreeC =(sensors.getTempC(thermThree));
    int thermFourC =(sensors.getTempC(thermFour));
    int difffront = sqrt(pow((thermOneC - thermTwoC),2));
    int diffrear = sqrt(pow((thermThreeC - thermFourC),2));
    int diff = max(difffront, diffrear);
    int maxfront = max(thermOneC, thermTwoC);
    int maxrear = max(thermThreeC, thermFourC);
    int maxall = max(maxfront, maxrear);
    Serial.print(difffront);
    Serial.print("front,");
    Serial.print(diffrear);
    Serial.print("rear,");
   
   /* if ((diff) > maxdiff) {//switches alarm on or off dependant upon diff in front OR rear temps - to modify the alarm temp difference change the value of maxdiff as defined above
        Serial.println("diff high");
        digitalWrite(alarm2, HIGH);
    } else {
        Serial.println("diff off");
        digitalWrite(alarm2, LOW);
    }
   */
    if ((maxall) > maxtemp) {//switches alarm on or off dependant upon max temp of ANY sensor - to modify the alarm temp difference change the value of maxtemp as defined above
        Serial.println("max temp high");
        digitalWrite(alarm, HIGH);
    } else {
        Serial.println("max temp off");
        digitalWrite(alarm, LOW);
    }
     ///////////////////////////////
 
 //  TFT update screen arcs///////////////////////

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


    delay(1000);//defines refresh speed
}
//___________