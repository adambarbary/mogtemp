// Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
// Interface design, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8
// Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials

// MOG PORTAL FLOW V4_final
// MX Temp and difference alarm
// 4 portal sensor display
// Adjustable brightness, difference setting, maximum temp
// Settings saved to ESP32 internal NVS flash (no SD card needed)
// Banner changes to warning on alarm
// Audio: LEDC PWM tone on GPIO 26 — avoids I2S/DAC which would claim
//        GPIO 25 (TOUCH_CLK) and permanently break touch input.

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
#include "actions.h"
#include "vars.h"
#include <Preferences.h>   // ESP32 built-in NVS flash storage

extern objects_t objects;
BB_SPI_LCD lcd;

Preferences prefs;   // NVS namespace handle

// ============================================================
// Pin Definitions - 2.8" ILI9341 CYD
// ============================================================
#define TOUCH_MISO    39
#define TOUCH_MOSI    32
#define TOUCH_CLK     25   // GPIO 25 — must NOT be claimed by I2S/DAC
#define TOUCH_CS      33
#define TFT_BL        21
#define SPEAKER_PIN   26   // GPIO 26 — LEDC PWM tone output

// ============================================================
// Display / LVGL
// ============================================================
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   320
#define DRAW_BUF_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
#define UI_UPDATE_DELAY 5
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
// Alarm tone pattern
// ============================================================
#define TONE_HIGH_HZ  1200
#define TONE_LOW_HZ    800
#define TONE_STEP_MS   400

// ============================================================
// Settings & State
// ============================================================
bool alarmOn           = true;
int  maxtemp           = 33;
int  maxdiff           = 4;
int  currentBrightness = 255;
volatile bool saveRequested = false;

// ============================================================
// Audio — Core 0 drives the LEDC tone pattern.
// Core 1 only sets/clears audioShouldPlay.
// ============================================================
volatile bool audioShouldPlay = false;
TaskHandle_t  audioTaskHandle = NULL;

// ============================================================
// Screen Brightness
// ============================================================
void setScreenBrightness(int brightness) {
  currentBrightness = constrain(brightness, 0, 255);
  analogWrite(TFT_BL, currentBrightness);
}

// ============================================================
// LVGL Logging
// ============================================================
void log_print(lv_log_level_t level, const char *buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// ============================================================
// Touchscreen Read Callback
// ============================================================
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data) {
  TOUCHINFO ti;
  if (lcd.rtReadTouch(&ti)) {
    data->point.x = constrain(240 - ti.x[0], 0, 239);
    data->point.y = constrain(ti.y[0], 0, 319);
    data->state   = LV_INDEV_STATE_PRESSED;
  } else {
    data->state   = LV_INDEV_STATE_RELEASED;
  }
}

// ============================================================
// LVGL Event Callbacks
// ============================================================
static void my_horizontal_slider_value_changed_event_cb(lv_event_t *e) {
  lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
  if (!slider || !lv_obj_is_valid(slider)) return;
  setScreenBrightness(map(lv_slider_get_value(slider), 0, 100, 0, 255));
}

static void max_temp_slider_value_changed_event_cb(lv_event_t *e) {
  lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
  if (!slider || !lv_obj_is_valid(slider)) return;
  char buf[8];
  maxtemp = (int)lv_slider_get_value(slider);
  lv_snprintf(buf, sizeof(buf), "%d", maxtemp);
  lv_label_set_text(objects.max_temp, buf);
}

static void max_diff_slider_value_changed_event_cb(lv_event_t *e) {
  lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
  if (!slider || !lv_obj_is_valid(slider)) return;
  char buf[8];
  maxdiff = (int)lv_slider_get_value(slider);
  lv_snprintf(buf, sizeof(buf), "%d", maxdiff);
  lv_label_set_text(objects.max_diff, buf);
}

static void alarm_switch_event_handler(lv_event_t *e) {
  if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
  lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e);
  if (!sw || !lv_obj_is_valid(sw)) return;
  alarmOn = lv_obj_has_state(sw, LV_STATE_CHECKED);
  lv_label_set_text(objects.alarm_switch_label, alarmOn ? "ON" : "OFF");
  lv_obj_set_style_text_color(objects.alarm_switch_label,
    alarmOn ? lv_color_hex(0xffff7f50) : lv_color_hex(0xffb6b6b6),
    LV_PART_MAIN | LV_STATE_DEFAULT);
}

extern "C" void action_save_settings(lv_event_t *e) {
  saveRequested = true;
}

// ============================================================
// NVS - Load / Save Settings
//
// prefs.begin("portal", true)  = read-only
// prefs.begin("portal", false) = read-write
//
// getX(key, default) returns the default if the key has never
// been written — handles a completely blank NVS gracefully.
// ESP32 NVS handles wear levelling automatically; it is rated
// for ~100,000 write cycles per key.
// ============================================================
void loadSettings() {
  prefs.begin("portal", true);   // read-only
  alarmOn           = prefs.getBool("alarmOn",    true);
  maxtemp           = prefs.getInt ("maxtemp",    33);
  maxdiff           = prefs.getInt ("maxdiff",    4);
  currentBrightness = prefs.getInt ("brightness", 255);
  prefs.end();

  // Clamp in case stored values are somehow out of range
  maxtemp           = constrain(maxtemp,           0, 100);
  maxdiff           = constrain(maxdiff,            0,  20);
  currentBrightness = constrain(currentBrightness,  0, 255);

  Serial.printf("Settings loaded: alarm=%d  maxtemp=%d  maxdiff=%d  brightness=%d\n",
    alarmOn, maxtemp, maxdiff, currentBrightness);
}

void saveSettings() {
  prefs.begin("portal", false);  // read-write
  prefs.putBool("alarmOn",    alarmOn);
  prefs.putInt ("maxtemp",    maxtemp);
  prefs.putInt ("maxdiff",    maxdiff);
  prefs.putInt ("brightness", currentBrightness);
  prefs.end();

  Serial.printf("Settings saved: alarm=%d  maxtemp=%d  maxdiff=%d  brightness=%d\n",
    alarmOn, maxtemp, maxdiff, currentBrightness);
}

// ============================================================
// Audio Task - Core 0
// LEDC PWM tone on GPIO 26 only. No I2S, no DAC, no GPIO 25.
// ============================================================
void audioTask(void *parameter) {
  Serial.println("[Audio] Task started on Core 0.");

  bool toneHigh   = true;
  bool wasPlaying = false;

  while (true) {
    if (audioShouldPlay) {
      if (!wasPlaying) {
        ledcAttach(SPEAKER_PIN, TONE_HIGH_HZ, 8);
        wasPlaying = true;
        toneHigh   = true;
        Serial.println("[Audio] Alarm tone started.");
      }

      ledcWriteTone(SPEAKER_PIN, toneHigh ? TONE_HIGH_HZ : TONE_LOW_HZ);
      toneHigh = !toneHigh;

      // Hold tone in short slices so alarm-off is noticed quickly
      uint32_t elapsed = 0;
      while (elapsed < TONE_STEP_MS && audioShouldPlay) {
        vTaskDelay(pdMS_TO_TICKS(20));
        elapsed += 20;
      }

    } else {
      if (wasPlaying) {
        ledcWriteTone(SPEAKER_PIN, 0);
        ledcDetach(SPEAKER_PIN);
        wasPlaying = false;
        Serial.println("[Audio] Alarm tone stopped.");
      }
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\nMog Portal Flow V4");

  // Load settings from NVS before anything else so all values
  // are correct when we configure the display and UI widgets.
  loadSettings();

  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);

  pinMode(TFT_BL, OUTPUT);
  setScreenBrightness(currentBrightness);

  // Display first — takes ownership of primary SPI bus
  lcd.begin(DISPLAY_CYD_2USB);
  lcd.setRotation(0);
  lcd.setFont(FONT_12x16);

  lv_init();
  lv_log_register_print_cb(log_print);

  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  if (!draw_buf) { Serial.println("FATAL: draw buffer"); while(1); }
  lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);

  lcd.rtInit(TOUCH_MOSI, TOUCH_MISO, TOUCH_CLK, TOUCH_CS);
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  ui_init();

  // Sync UI widgets to loaded/default values
  char buf[8];
  lv_slider_set_value(objects.max_temp_slider,      maxtemp,                                LV_ANIM_OFF);
  lv_slider_set_value(objects.max_diff_slider,      maxdiff,                                LV_ANIM_OFF);
  lv_slider_set_value(objects.my_horizontal_slider, map(currentBrightness, 0, 255, 0, 100), LV_ANIM_OFF);
  lv_snprintf(buf, sizeof(buf), "%d", maxtemp); lv_label_set_text(objects.max_temp, buf);
  lv_snprintf(buf, sizeof(buf), "%d", maxdiff); lv_label_set_text(objects.max_diff, buf);

  if (alarmOn) {
    lv_obj_add_state(objects.alarm_switch, LV_STATE_CHECKED);
    lv_label_set_text(objects.alarm_switch_label, "ON");
    lv_obj_set_style_text_color(objects.alarm_switch_label,
      lv_color_hex(0xffff7f50), LV_PART_MAIN | LV_STATE_DEFAULT);
  } else {
    lv_obj_clear_state(objects.alarm_switch, LV_STATE_CHECKED);
    lv_label_set_text(objects.alarm_switch_label, "OFF");
    lv_obj_set_style_text_color(objects.alarm_switch_label,
      lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
  }

  lv_obj_add_event_cb(objects.my_horizontal_slider, my_horizontal_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.max_temp_slider,      max_temp_slider_value_changed_event_cb,      LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.max_diff_slider,      max_diff_slider_value_changed_event_cb,      LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.alarm_switch,         alarm_switch_event_handler,                  LV_EVENT_VALUE_CHANGED, NULL);

  sensors.begin();
  sensors.setResolution(thermOne,   TEMPERATURE_PRECISION);
  sensors.setResolution(thermTwo,   TEMPERATURE_PRECISION);
  sensors.setResolution(thermThree, TEMPERATURE_PRECISION);
  sensors.setResolution(thermFour,  TEMPERATURE_PRECISION);

  // Audio task on Core 0; LVGL/UI on Core 1
  xTaskCreatePinnedToCore(audioTask, "audioTask", 4096, NULL, 1, &audioTaskHandle, 0);

  Serial.println("Setup complete.");
}

// ============================================================
// Loop - Core 1
// ============================================================
void loop() {
  static uint32_t      lastTickLocal = 0;
  static unsigned long lastTempRead  = 0;

  lv_tick_inc(millis() - lastTickLocal);
  lastTickLocal = millis();
  lv_timer_handler();
  eez_flow_tick();

  if (saveRequested) {
    saveRequested = false;
    saveSettings();   // fast NVS write, no SPI bus, no blocking
  }

  if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
    lastTempRead = millis();

    sensors.requestTemperatures();
    int t1 = (int)sensors.getTempC(thermOne);
    int t2 = (int)sensors.getTempC(thermTwo);
    int t3 = (int)sensors.getTempC(thermThree);
    int t4 = (int)sensors.getTempC(thermFour);

    bool alarmTriggered = alarmOn &&
      ((max(max(t1,t2), max(t3,t4)) > maxtemp) ||
       (max(abs(t1-t2), abs(t3-t4)) > maxdiff));

    if (alarmTriggered) {
      set_var_alarm_panel("HIGH");
      eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_ALARM_PANEL,
        eez::Value("HIGH", eez::VALUE_TYPE_STRING));
      lv_timer_handler();
      eez_flow_tick();
      audioShouldPlay = true;
    } else {
      audioShouldPlay = false;
      set_var_alarm_panel("LOW");
      eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_ALARM_PANEL,
        eez::Value("LOW", eez::VALUE_TYPE_STRING));
    }

    char buf[5];
    lv_snprintf(buf, sizeof(buf), "%d", t1);
    lv_arc_set_value(objects.fl_portal_guage, t1);
    lv_label_set_text(objects.fl_portal_label, buf);

    lv_snprintf(buf, sizeof(buf), "%d", t2);
    lv_arc_set_value(objects.fr_portal_guage, t2);
    lv_label_set_text(objects.fr_portal_label, buf);

    lv_snprintf(buf, sizeof(buf), "%d", t3);
    lv_arc_set_value(objects.rl_portal_guage, t3);
    lv_label_set_text(objects.rl_portal_label, buf);

    lv_snprintf(buf, sizeof(buf), "%d", t4);
    lv_arc_set_value(objects.rr_portal_guage, t4);
    lv_label_set_text(objects.rr_portal_label, buf);
  }

  delay(UI_UPDATE_DELAY);
}