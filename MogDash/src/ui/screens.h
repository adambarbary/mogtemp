#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *settings;
    lv_obj_t *probes;
    lv_obj_t *settings_panel;
    lv_obj_t *settings_switch_panel;
    lv_obj_t *save_back;
    lv_obj_t *back;
    lv_obj_t *probe_link;
    lv_obj_t *settings_switch_panel_1;
    lv_obj_t *probe_save_back;
    lv_obj_t *probes_return;
    lv_obj_t *search_icon;
    lv_obj_t *background;
    lv_obj_t *banner_alert;
    lv_obj_t *banner_alert_text;
    lv_obj_t *front_left_container;
    lv_obj_t *fl_portal_guage;
    lv_obj_t *fl_portal_label;
    lv_obj_t *front_right_container;
    lv_obj_t *fr_portal_guage;
    lv_obj_t *fr_portal_label;
    lv_obj_t *rear_left_container;
    lv_obj_t *rl_portal_guage;
    lv_obj_t *rl_portal_label;
    lv_obj_t *rear_right_container;
    lv_obj_t *rr_portal_guage;
    lv_obj_t *rr_portal_label;
    lv_obj_t *my_horizontal_slider;
    lv_obj_t *my_panel_header_1;
    lv_obj_t *settings_label;
    lv_obj_t *alarm_switch;
    lv_obj_t *alarm_switch_label;
    lv_obj_t *setting_label_alarm;
    lv_obj_t *max_temp_slider;
    lv_obj_t *max_temp_slider_label;
    lv_obj_t *max_diff_slider;
    lv_obj_t *max_diff_slider_label;
    lv_obj_t *max_temp;
    lv_obj_t *max_diff;
    lv_obj_t *my_probes_heading;
    lv_obj_t *probes_label_header;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *portal1;
    lv_obj_t *portal4;
    lv_obj_t *portal3;
    lv_obj_t *portal2;
    lv_obj_t *probes_container_sensor1;
    lv_obj_t *probes_address_1;
    lv_obj_t *probes_container_sensor2;
    lv_obj_t *probes_address_2;
    lv_obj_t *probes_container_sensor3;
    lv_obj_t *probes_address_3;
    lv_obj_t *probes_container_sensor4;
    lv_obj_t *probes_address_4;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SETTINGS = 2,
    SCREEN_ID_PROBES = 3,
};

void create_screen_main();
void tick_screen_main();

void create_screen_settings();
void tick_screen_settings();

void create_screen_probes();
void tick_screen_probes();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/