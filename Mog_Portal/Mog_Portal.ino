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

// ---------- Sensor addresses (mutable — loaded from NVS or defaulted) ----------
// These are no longer const so they can be reassigned from the Probes page.
// Defaults are the original hard-coded values and are used only when NVS has no
// saved addresses (i.e. first boot or after a flash-erase).
DeviceAddress thermOne   = { 0x28,0xA0,0x22,0xAA,0x7E,0x25,0x0B,0x8E };
DeviceAddress thermTwo   = { 0x28,0xEC,0x68,0x9D,0x7E,0x25,0x0B,0x1C };
DeviceAddress thermThree = { 0x28,0x22,0x78,0x67,0x7E,0x25,0x0B,0x86 };
DeviceAddress thermFour  = { 0x28,0xC9,0xFB,0x7C,0x7E,0x25,0x0B,0x7E };

// ---------- Probe search results (temporary, not persisted) ----------
// Populated by action_start_search; consumed by action_save_probes.
DeviceAddress foundAddresses[4];
int           foundCount = 0;

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
volatile bool saveRequested      = false;
volatile bool saveProbesRequested = false;  // separate flag — only writes addresses

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
// Address Formatting Helpers
// ============================================================

// Format a DeviceAddress as the human-readable string shown on the Probes page.
// Example output: "28 A0 22 AA 7E 25 0B 8E"  (bufLen should be >= 24)
static void formatAddressDisplay(const DeviceAddress addr, char *buf, size_t bufLen) {
  snprintf(buf, bufLen,
    "%02X %02X %02X %02X %02X %02X %02X %02X",
    addr[0], addr[1], addr[2], addr[3],
    addr[4], addr[5], addr[6], addr[7]);
}

// ============================================================
// NVS - Load / Save Settings (alarm, temp limits, brightness)
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
// NVS - Load / Save Probe Addresses (independent of other settings)
//
// Each address is stored as 8 raw bytes under keys "addr1".."addr4".
// getBytesLength() is used to check whether a key exists before
// reading so that we keep the hard-coded defaults on first boot.
// ============================================================
void loadProbeAddresses() {
  prefs.begin("portal", true);   // read-only
  if (prefs.getBytesLength("addr1") == 8) prefs.getBytes("addr1", thermOne,   8);
  if (prefs.getBytesLength("addr2") == 8) prefs.getBytes("addr2", thermTwo,   8);
  if (prefs.getBytesLength("addr3") == 8) prefs.getBytes("addr3", thermThree, 8);
  if (prefs.getBytesLength("addr4") == 8) prefs.getBytes("addr4", thermFour,  8);
  prefs.end();

  char buf[24];
  formatAddressDisplay(thermOne,   buf, sizeof(buf)); Serial.printf("Probe 1 loaded: %s\n", buf);
  formatAddressDisplay(thermTwo,   buf, sizeof(buf)); Serial.printf("Probe 2 loaded: %s\n", buf);
  formatAddressDisplay(thermThree, buf, sizeof(buf)); Serial.printf("Probe 3 loaded: %s\n", buf);
  formatAddressDisplay(thermFour,  buf, sizeof(buf)); Serial.printf("Probe 4 loaded: %s\n", buf);
}

void saveProbeAddresses() {
  prefs.begin("portal", false);  // read-write
  prefs.putBytes("addr1", thermOne,   8);
  prefs.putBytes("addr2", thermTwo,   8);
  prefs.putBytes("addr3", thermThree, 8);
  prefs.putBytes("addr4", thermFour,  8);
  prefs.end();

  char buf[24];
  formatAddressDisplay(thermOne,   buf, sizeof(buf)); Serial.printf("Probe 1 saved: %s\n", buf);
  formatAddressDisplay(thermTwo,   buf, sizeof(buf)); Serial.printf("Probe 2 saved: %s\n", buf);
  formatAddressDisplay(thermThree, buf, sizeof(buf)); Serial.printf("Probe 3 saved: %s\n", buf);
  formatAddressDisplay(thermFour,  buf, sizeof(buf)); Serial.printf("Probe 4 saved: %s\n", buf);
}

// ============================================================
// Native Action: Start_Search
//
// Scans the OneWire bus, populates the four probes_address labels
// on the Probes page, and updates all four portal dropdown menus
// so the user can assign each found probe to a sensor slot.
//
// Dropdown options are built as:
//   "None\nProbe 1\nProbe 2\n..." — "None" is always index 0.
// ============================================================
extern "C" void action_start_search(lv_event_t *e) {
  // Rescan the bus — getDeviceCount() triggers a fresh search
  sensors.begin();
  foundCount = (int)sensors.getDeviceCount();
  if (foundCount > 4) foundCount = 4;

  Serial.printf("[Probe Search] Found %d sensor(s)\n", foundCount);

  // Label pointers in order — easier to iterate than four separate calls
  lv_obj_t *addrLabels[4] = {
    objects.probes_address_1,
    objects.probes_address_2,
    objects.probes_address_3,
    objects.probes_address_4
  };

  char addrBuf[24];   // "28 A0 22 AA 7E 25 0B 8E" = 23 chars + NUL

  // Reset all labels to dashes first, then fill in what we found
  for (int i = 0; i < 4; i++) {
    lv_label_set_text(addrLabels[i], "---");
  }

  // Build the dropdown options string: "None\nProbe 1\nProbe 2\n..."
  // Maximum length: "None" + 4 × "\nProbe N" = 4 + 4*8 = 36 chars + NUL — fits in 64
  //char dropdownOpts[64] = "[None]";
  char dropdownOpts[64] = "---";
  for (int i = 0; i < foundCount; i++) {
    if (sensors.getAddress(foundAddresses[i], i)) {
      formatAddressDisplay(foundAddresses[i], addrBuf, sizeof(addrBuf));
      lv_label_set_text(addrLabels[i], addrBuf);
      Serial.printf("  Probe %d: %s\n", i + 1, addrBuf);

      // Append this probe to the dropdown option list
      char entry[8];
      snprintf(entry, sizeof(entry), "\nP%d", i + 1);
      strncat(dropdownOpts, entry, sizeof(dropdownOpts) - strlen(dropdownOpts) - 1);
    } else {
      Serial.printf("  Probe %d: getAddress() failed\n", i + 1);
    }
  }

  // Push identical option lists to all four assignment dropdowns
  lv_dropdown_set_options(objects.portal1, dropdownOpts);
  lv_dropdown_set_options(objects.portal2, dropdownOpts);
  lv_dropdown_set_options(objects.portal3, dropdownOpts);
  lv_dropdown_set_options(objects.portal4, dropdownOpts);

  // Reset selections to "None" so the user makes an explicit choice
  lv_dropdown_set_selected(objects.portal1, 0);
  lv_dropdown_set_selected(objects.portal2, 0);
  lv_dropdown_set_selected(objects.portal3, 0);
  lv_dropdown_set_selected(objects.portal4, 0);
}

// ============================================================
// Native Action: save_probes
//
// Reads the four portal dropdown selections, maps each chosen
// "Probe N" back to the corresponding entry in foundAddresses[],
// copies the bytes into the thermOne..thermFour variables, then
// sets saveProbesRequested so the loop persists them to NVS.
//
// Selecting "None" (index 0) leaves that therm variable unchanged.
// ============================================================
extern "C" void action_save_probes(lv_event_t *e) {
  // Dropdown index 0 = "None", index 1 = "Probe 1", etc.
  // Subtract 1 to convert to a zero-based foundAddresses index.
  int sel[4] = {
    (int)lv_dropdown_get_selected(objects.portal1) - 1,
    (int)lv_dropdown_get_selected(objects.portal2) - 1,
    (int)lv_dropdown_get_selected(objects.portal3) - 1,
    (int)lv_dropdown_get_selected(objects.portal4) - 1
  };

  DeviceAddress *therms[4] = { &thermOne, &thermTwo, &thermThree, &thermFour };

  for (int i = 0; i < 4; i++) {
    if (sel[i] >= 0 && sel[i] < foundCount) {
      memcpy(therms[i], foundAddresses[sel[i]], 8);

      char buf[24];
      formatAddressDisplay(*therms[i], buf, sizeof(buf));
      Serial.printf("Slot %d assigned to Probe %d: %s\n", i + 1, sel[i] + 1, buf);
    }
  }

  // Re-apply resolution to all slots (some addresses may have changed)
  sensors.setResolution(thermOne,   TEMPERATURE_PRECISION);
  sensors.setResolution(thermTwo,   TEMPERATURE_PRECISION);
  sensors.setResolution(thermThree, TEMPERATURE_PRECISION);
  sensors.setResolution(thermFour,  TEMPERATURE_PRECISION);

  // Persist addresses on the next loop iteration (keeps NVS write off the event thread)
  saveProbesRequested = true;
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
  Serial.println("\nMog Portal");

  // Load all settings from NVS before anything else so every value
  // is correct when we configure the display and UI widgets.
  loadSettings();
  loadProbeAddresses();   // must come after loadSettings (same NVS namespace, sequential opens are fine)

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

  // Prevent full-screen background panels from swallowing taps on empty space —
  // their PRESSED handler calls into the EEZ flow engine with an unconnected (-1)
  // output index, which crashes. These panels don't need to be clickable.
  lv_obj_clear_flag(objects.settings_switch_panel,   LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(objects.settings_switch_panel_1, LV_OBJ_FLAG_CLICKABLE);

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

  // Initialise the Probes page dropdowns with a placeholder option so they
  // are not blank before the user triggers a search.
  const char *unsearchedOpts = "Run Search First";
  lv_dropdown_set_options(objects.portal1, unsearchedOpts);
  lv_dropdown_set_options(objects.portal2, unsearchedOpts);
  lv_dropdown_set_options(objects.portal3, unsearchedOpts);
  lv_dropdown_set_options(objects.portal4, unsearchedOpts);

  lv_obj_add_event_cb(objects.my_horizontal_slider, my_horizontal_slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.max_temp_slider,      max_temp_slider_value_changed_event_cb,      LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.max_diff_slider,      max_diff_slider_value_changed_event_cb,      LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.alarm_switch,         alarm_switch_event_handler,                  LV_EVENT_VALUE_CHANGED, NULL);

  // Sensor init — uses the addresses loaded from NVS (or defaults on first boot)
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
    saveSettings();         // writes alarm/temp/diff/brightness — not addresses
  }

  if (saveProbesRequested) {
    saveProbesRequested = false;
    saveProbeAddresses();   // writes only the four sensor addresses
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
      lv_timer_handler();
      eez_flow_tick();
      audioShouldPlay = true;
    } else {
      audioShouldPlay = false;
      set_var_alarm_panel("LOW");
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