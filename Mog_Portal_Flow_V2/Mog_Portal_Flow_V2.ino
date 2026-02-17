// Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
// based on: http://swingleydev.org/blog/tag/arduino/ (no longer available)
// Interface design, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8
// Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials
// Save icons created by Yogi Aprelliyanto - Flaticon https://www.flaticon.com/free-icons/save
// Return icons created by Kiranshastry - Flaticon https://www.flaticon.com/free-icons/return

// MOG PORTA FLOW V2 
// MX Temp and difference alarm
// 4 portal sensor display
// Adjustable brightness
// Adjustable difference setting
// Adjustable maximum temp
// Save/Load to/from SD Card

// ============================================================
// Included Libraries
// ============================================================
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ui.h"
#include "screens.h"
#include <lvgl.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <bb_spi_lcd.h>
#include <SD.h>
#include <FS.h>

extern objects_t objects;

BB_SPI_LCD lcd;

// ============================================================
// Pin Definitions - 2.8" ILI9341 CYD
// ============================================================
#define TOUCH_MISO    39
#define TOUCH_MOSI    32
#define TOUCH_CLK     25
#define TOUCH_CS      33
#define TFT_BL        21

// ============================================================
// Display / LVGL
// ============================================================
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint8_t *draw_buf;

// ============================================================
// Temperature Sensors
// ============================================================
#define ONE_WIRE_BUS          27
#define TEMPERATURE_PRECISION 12
#define TEMP_READ_INTERVAL    4000

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const DeviceAddress thermOne   = { 0x28,0xE1,0xB9,0x78,0x70,0x23,0x6,0xAE };
const DeviceAddress thermTwo   = { 0x28,0x8D,0x30,0x44,0x70,0x23,0x6,0x72 };
const DeviceAddress thermThree = { 0x28,0xE8,0x7D,0x5C,0x70,0x23,0x6,0x7B };
const DeviceAddress thermFour  = { 0x28,0x56,0xDC,0x98,0x70,0x23,0x6,0xA6 };

// ============================================================
// Alarm / Speaker
// ============================================================
#define alarm           22
#define ALARM_OFF       LOW
#define ALARM_ON        HIGH
#define UI_UPDATE_DELAY 5

const int speakerPin = 26;
const int pwmChannel = 0;

// ============================================================
// Settings & State
// ============================================================
const int chipSelect   = 5;    // CYD SD CS pin (VSPI)
SPIClass sdSPI(VSPI);          // SD card on VSPI, separate from display HSPI
bool sdAvailable       = false;
bool alarmOn           = true;
int  maxtemp           = 33;
int  maxdiff           = 4;
int  currentBrightness = 255;

// FIX: Flag set by the LVGL save button callback, acted on
// safely in loop() - keeps SD writes out of the LVGL/EEZ
// dispatch stack entirely.
volatile bool saveRequested = false;

// ============================================================
// LVGL Logging
// ============================================================
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// ============================================================
// Touchscreen Read Callback
// ============================================================
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  TOUCHINFO ti;
  if (lcd.rtReadTouch(&ti)) {
    data->point.x = 240 - ti.x[0];
    data->point.y = ti.y[0];
    data->state   = LV_INDEV_STATE_PRESSED;
  } else {
    data->state   = LV_INDEV_STATE_RELEASED;
  }
}

// ============================================================
// Screen Brightness
// ============================================================
void setScreenBrightness(int brightness) {
  brightness        = constrain(brightness, 0, 255);
  analogWrite(TFT_BL, brightness);
  currentBrightness = brightness;
}

// ============================================================
// LVGL Event Callbacks
// All callbacks read from the global objects struct directly.
// No lv_obj_t* pointers are stored or passed as user_data.
// ============================================================

static void my_horizontal_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
  if (!slider || !lv_obj_is_valid(slider)) return;
  setScreenBrightness(map(lv_slider_get_value(slider), 0, 100, 0, 255));
}

static void max_temp_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
  if (!slider || !lv_obj_is_valid(slider)) return;
  if (!objects.max_temp || !lv_obj_is_valid(objects.max_temp)) return;

  maxtemp = (int)lv_slider_get_value(slider);
  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", maxtemp);
  lv_label_set_text(objects.max_temp, buf);
}

static void max_diff_slider_value_changed_event_cb(lv_event_t * e) {
  lv_obj_t * slider = (lv_obj_t *)lv_event_get_target(e);
  if (!slider || !lv_obj_is_valid(slider)) return;
  if (!objects.max_diff || !lv_obj_is_valid(objects.max_diff)) return;

  maxdiff = (int)lv_slider_get_value(slider);
  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", maxdiff);
  lv_label_set_text(objects.max_diff, buf);
}

static void alarm_switch_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
  lv_obj_t * sw = (lv_obj_t *)lv_event_get_target(e);
  if (!sw || !lv_obj_is_valid(sw)) return;
  if (!objects.alarm_switch_label || !lv_obj_is_valid(objects.alarm_switch_label)) return;

  alarmOn = lv_obj_has_state(sw, LV_STATE_CHECKED);
  lv_label_set_text(objects.alarm_switch_label, alarmOn ? "ON" : "OFF");
  lv_obj_set_style_text_color(objects.alarm_switch_label,
    alarmOn ? lv_color_hex(0xffff7f50) : lv_color_hex(0xffb6b6b6),
    LV_PART_MAIN | LV_STATE_DEFAULT);
}

// FIX: Correct signature matching actions.h / ActionExecFunc.
// EEZ calls all actions with an lv_event_t* argument.
// The previous void action_save_settings() with NO parameter
// was the root cause of every crash — EEZ passed the event pointer
// in register A2 but the mismatched prologue corrupted the stack
// frame and return address on every single call.
// We only set a flag here; the actual save runs safely in loop()
// outside the EEZ/LVGL dispatch stack.
extern "C" void action_save_settings(lv_event_t * e) {
  saveRequested = true;
  Serial.println("Save requested via EEZ action (will execute in loop).");
}

// ============================================================
// SD Card - Load / Save Settings
// ============================================================
void loadSettings() {
  if (!SD.exists("/config.txt")) {
    Serial.println("No config file found, using defaults.");
    return;
  }

  File f = SD.open("/config.txt", FILE_READ);
  if (!f) {
    Serial.println("Config file found but could not be opened. Using defaults.");
    return;
  }

  Serial.println("Loading settings from SD card...");
  f.setTimeout(100);

  alarmOn           = (bool)f.parseInt();
  maxtemp           = f.parseInt();
  maxdiff           = f.parseInt();
  currentBrightness = f.parseInt();
  f.close();

  maxtemp           = constrain(maxtemp,           0, 100);
  maxdiff           = constrain(maxdiff,            0,  20);
  currentBrightness = constrain(currentBrightness,  0, 255);

  Serial.println("Settings loaded:");
  Serial.print("  alarmOn:           "); Serial.println(alarmOn);
  Serial.print("  maxtemp:           "); Serial.println(maxtemp);
  Serial.print("  maxdiff:           "); Serial.println(maxdiff);
  Serial.print("  currentBrightness: "); Serial.println(currentBrightness);
}

void saveSettings() {
  if (!sdAvailable) {
    Serial.println("SD card not available - settings not saved.");
    return;
  }

  if (SD.exists("/config.txt")) SD.remove("/config.txt");

  File f = SD.open("/config.txt", FILE_WRITE);
  if (!f) {
    Serial.println("Error opening config file for writing.");
    return;
  }

  f.println(alarmOn);
  f.println(maxtemp);
  f.println(maxdiff);
  f.println(currentBrightness);
  f.close();

  Serial.println("Settings saved:");
  Serial.print("  alarmOn:           "); Serial.println(alarmOn);
  Serial.print("  maxtemp:           "); Serial.println(maxtemp);
  Serial.print("  maxdiff:           "); Serial.println(maxdiff);
  Serial.print("  currentBrightness: "); Serial.println(currentBrightness);
}

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\nMog Portal Dash");
  Serial.print("Reset reason: ");
  Serial.println(esp_reset_reason());

  ledcAttachChannel(speakerPin, 2000, 8, pwmChannel);

  pinMode(alarm, OUTPUT);
  digitalWrite(alarm, ALARM_OFF);

  pinMode(TFT_BL, OUTPUT);
  setScreenBrightness(currentBrightness);

  Serial.printf("LVGL Library Version: %d.%d.%d\n",
    lv_version_major(), lv_version_minor(), lv_version_patch());
  Serial.println("Starting LVGL setup...");

  // Display initialised FIRST — bb_spi_lcd takes ownership of
  // the primary SPI bus. SD card is initialised afterwards on
  // a separate SPI instance so the two do not conflict.
  lcd.begin(DISPLAY_CYD_2USB);
  lcd.setRotation(0);
  lcd.setFont(FONT_12x16);

  // SD card on VSPI (SCK=18, MISO=19, MOSI=23, CS=5).
  // Display and touch are on HSPI (SCK=14, MOSI=13, CS=15)
  // via bb_spi_lcd. Keeping them on separate buses is the
  // only reliable way to use SD + display + touch on the CYD.
  Serial.print("Initializing SD card... ");
  sdSPI.begin(18, 19, 23, chipSelect);  // SCK, MISO, MOSI, CS
  if (!SD.begin(chipSelect, sdSPI, 25000000)) {
    Serial.println("failed! Running with defaults.");
  } else {
    Serial.println("OK.");
    sdAvailable = true;
    loadSettings();
  }

  lv_init();
  lv_log_register_print_cb(log_print);

  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  if (!draw_buf) {
    Serial.println("FATAL: Failed to allocate draw buffer!");
    while (1);
  }
  lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);

  lcd.rtInit(TOUCH_MOSI, TOUCH_MISO, TOUCH_CLK, TOUCH_CS);
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  Serial.println("LVGL setup complete.");

  ui_init();

  // ---- Sync all UI widgets to loaded (or default) values ----
  lv_slider_set_value(objects.max_temp_slider,      maxtemp,                                LV_ANIM_OFF);
  lv_slider_set_value(objects.max_diff_slider,      maxdiff,                                LV_ANIM_OFF);
  lv_slider_set_value(objects.my_horizontal_slider, map(currentBrightness, 0, 255, 0, 100), LV_ANIM_OFF);

  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d", maxtemp);
  lv_label_set_text(objects.max_temp, buf);
  lv_snprintf(buf, sizeof(buf), "%d", maxdiff);
  lv_label_set_text(objects.max_diff, buf);

  if (alarmOn) {
    lv_obj_add_state(objects.alarm_switch, LV_STATE_CHECKED);
    lv_label_set_text(objects.alarm_switch_label, "ON");
    lv_obj_set_style_text_color(objects.alarm_switch_label, lv_color_hex(0xffff7f50), LV_PART_MAIN | LV_STATE_DEFAULT);
  } else {
    lv_obj_clear_state(objects.alarm_switch, LV_STATE_CHECKED);
    lv_label_set_text(objects.alarm_switch_label, "OFF");
    lv_obj_set_style_text_color(objects.alarm_switch_label, lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
  }

  // ---- Register event callbacks ----
  // EEZ Flow handles the save button dispatch via action_save_settings().
  // Only slider and switch callbacks need manual registration here.
  lv_obj_add_event_cb(objects.my_horizontal_slider, my_horizontal_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.max_temp_slider,      max_temp_slider_value_changed_event_cb,      LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.max_diff_slider,      max_diff_slider_value_changed_event_cb,      LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.alarm_switch,         alarm_switch_event_handler,                  LV_EVENT_VALUE_CHANGED, NULL);

  sensors.begin();
  sensors.setResolution(thermOne,   TEMPERATURE_PRECISION);
  sensors.setResolution(thermTwo,   TEMPERATURE_PRECISION);
  sensors.setResolution(thermThree, TEMPERATURE_PRECISION);
  sensors.setResolution(thermFour,  TEMPERATURE_PRECISION);

  Serial.println("Setup complete - watching for interactions...");
}

// ============================================================
// Loop
// ============================================================
void loop() {
  static uint32_t      lastTickLocal = 0;
  static unsigned long lastTempRead  = 0;
  static unsigned long lastBeepTime  = 0;

  lv_tick_inc(millis() - lastTickLocal);
  lastTickLocal = millis();
  lv_timer_handler();
  eez_flow_tick();

  // FIX: Process deferred save here, completely outside the
  // LVGL/EEZ call stack. Safe to do file I/O at this point.
  if (saveRequested) {
    saveRequested = false;
    saveSettings();
  }

  if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
    lastTempRead = millis();

    sensors.requestTemperatures();
    int thermOneC   = (int)sensors.getTempC(thermOne);
    int thermTwoC   = (int)sensors.getTempC(thermTwo);
    int thermThreeC = (int)sensors.getTempC(thermThree);
    int thermFourC  = (int)sensors.getTempC(thermFour);

    int diff   = max(abs(thermOneC - thermTwoC), abs(thermThreeC - thermFourC));
    int maxall = max(max(thermOneC, thermTwoC), max(thermThreeC, thermFourC));

    bool alarmTriggered = alarmOn && ((maxall > maxtemp) || (diff > maxdiff));

    if (alarmTriggered) {
      digitalWrite(alarm, ALARM_ON);
      if (millis() - lastBeepTime >= 700) {
        ledcWriteTone(pwmChannel, 2500);
        lastBeepTime = millis();
      }
    } else {
      digitalWrite(alarm, ALARM_OFF);
      ledcWriteTone(pwmChannel, 0);
    }

    char buf[5];

    lv_snprintf(buf, sizeof(buf), "%d", thermOneC);
    lv_arc_set_value(objects.fl_portal_guage, thermOneC);
    lv_label_set_text(objects.fl_portal_label, buf);

    lv_snprintf(buf, sizeof(buf), "%d", thermTwoC);
    lv_arc_set_value(objects.fr_portal_guage, thermTwoC);
    lv_label_set_text(objects.fr_portal_label, buf);

    lv_snprintf(buf, sizeof(buf), "%d", thermThreeC);
    lv_arc_set_value(objects.rl_portal_guage, thermThreeC);
    lv_label_set_text(objects.rl_portal_label, buf);

    lv_snprintf(buf, sizeof(buf), "%d", thermFourC);
    lv_arc_set_value(objects.rr_portal_guage, thermFourC);
    lv_label_set_text(objects.rr_portal_label, buf);
  }

  delay(UI_UPDATE_DELAY);
}