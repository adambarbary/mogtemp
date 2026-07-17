#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_main(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_SCREEN_LOADED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 1, 0, e);
    }
}

static void event_handler_cb_main_settings_panel(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 17, 0, e);
    }
}

static void event_handler_cb_settings_settings_switch_panel(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 4, e);
    }
}

static void event_handler_cb_settings_save_back(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        action_save_settings(e);
    }
    if (event == LV_EVENT_RELEASED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 13, 1, e);
    }
}

static void event_handler_cb_settings_back(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 14, 0, e);
    }
}

static void event_handler_cb_settings_probe_link(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 15, 0, e);
    }
}

static void event_handler_cb_probes_probes(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        action_save_probes(e);
    }
}

static void event_handler_cb_probes_settings_switch_panel_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 4, e);
    }
}

static void event_handler_cb_probes_probe_save_back(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        action_save_probes(e);
    }
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 4, 0, e);
    }
}

static void event_handler_cb_probes_probes_return(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 5, 0, e);
    }
}

static void event_handler_cb_probes_search_icon(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        action_start_search(e);
    }
}

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, event_handler_cb_main_main, LV_EVENT_ALL, flowState);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // background
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.background = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_mog);
            lv_image_set_scale(obj, 168);
            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_DEFAULT);
            lv_obj_set_style_transform_scale_x(obj, 256, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_transform_scale_y(obj, 256, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // banner_alert
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.banner_alert = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 33);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // banner_alert_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.banner_alert_text = obj;
                    lv_obj_set_pos(obj, 37, 8);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Unimog Portal Temp");
                }
            }
        }
        {
            // front_left_container
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.front_left_container = obj;
            lv_obj_set_pos(obj, 0, 56);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fl_portal_guage
                    lv_obj_t *obj = lv_arc_create(parent_obj);
                    objects.fl_portal_guage = obj;
                    lv_obj_set_pos(obj, 2, 2);
                    lv_obj_set_size(obj, 74, 74);
                    lv_arc_set_range(obj, 0, 60);
                    lv_arc_set_value(obj, 60);
                    lv_obj_set_style_arc_image_src(obj, &img_gradient, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_width(obj, 15, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 50, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // fl_portal_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.fl_portal_label = obj;
                            lv_obj_set_pos(obj, 25, 24);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "60");
                        }
                    }
                }
            }
        }
        {
            // front_right_container
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.front_right_container = obj;
            lv_obj_set_pos(obj, 160, 55);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fr_portal_guage
                    lv_obj_t *obj = lv_arc_create(parent_obj);
                    objects.fr_portal_guage = obj;
                    lv_obj_set_pos(obj, 0, 3);
                    lv_obj_set_size(obj, 74, 74);
                    lv_arc_set_range(obj, 0, 60);
                    lv_arc_set_value(obj, 60);
                    lv_obj_set_style_arc_image_src(obj, &img_gradient, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_width(obj, 15, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 50, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // fr_portal_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.fr_portal_label = obj;
                            lv_obj_set_pos(obj, 22, 24);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "60");
                        }
                    }
                }
            }
        }
        {
            // rear_left_container
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.rear_left_container = obj;
            lv_obj_set_pos(obj, 0, 176);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // rl_portal_guage
                    lv_obj_t *obj = lv_arc_create(parent_obj);
                    objects.rl_portal_guage = obj;
                    lv_obj_set_pos(obj, 2, 2);
                    lv_obj_set_size(obj, 74, 74);
                    lv_arc_set_range(obj, 0, 60);
                    lv_arc_set_value(obj, 60);
                    lv_obj_set_style_arc_image_src(obj, &img_gradient, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_width(obj, 15, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 50, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // rl_portal_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.rl_portal_label = obj;
                            lv_obj_set_pos(obj, 24, 25);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "60");
                        }
                    }
                }
            }
        }
        {
            // rear_right_container
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.rear_right_container = obj;
            lv_obj_set_pos(obj, 160, 176);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // rr_portal_guage
                    lv_obj_t *obj = lv_arc_create(parent_obj);
                    objects.rr_portal_guage = obj;
                    lv_obj_set_pos(obj, 0, 2);
                    lv_obj_set_size(obj, 74, 74);
                    lv_arc_set_range(obj, 0, 60);
                    lv_arc_set_value(obj, 60);
                    lv_obj_set_style_arc_image_src(obj, &img_gradient, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_width(obj, 15, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 50, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // rr_portal_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.rr_portal_label = obj;
                            lv_obj_set_pos(obj, 22, 25);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "60");
                        }
                    }
                }
            }
        }
        {
            // my_horizontal_slider
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.my_horizontal_slider = obj;
            lv_obj_set_pos(obj, 11, 295);
            lv_obj_set_size(obj, 163, 10);
            lv_slider_set_range(obj, 0, 99);
            lv_slider_set_value(obj, 50, LV_ANIM_OFF);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 5, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_KNOB | LV_STATE_PRESSED);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_KNOB | LV_STATE_PRESSED);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_KNOB | LV_STATE_PRESSED);
            lv_obj_set_style_pad_right(obj, 8, LV_PART_KNOB | LV_STATE_PRESSED);
        }
        {
            // settings_panel
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.settings_panel = obj;
            lv_obj_set_pos(obj, 190, 280);
            lv_obj_set_size(obj, 40, 40);
            lv_image_set_src(obj, &img_gear);
            lv_image_set_scale(obj, 200);
            lv_obj_add_event_cb(obj, event_handler_cb_main_settings_panel, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}

void create_screen_settings() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // my_panel_header_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.my_panel_header_1 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 33);
            add_style_style_normal(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // settings_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.settings_label = obj;
            lv_obj_set_pos(obj, 83, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Settings");
        }
        {
            // settings_switch_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_switch_panel = obj;
            lv_obj_set_pos(obj, 0, 40);
            lv_obj_set_size(obj, 240, 280);
            lv_obj_add_event_cb(obj, event_handler_cb_settings_settings_switch_panel, LV_EVENT_ALL, flowState);
            add_style_style_normal(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // alarm_switch
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.alarm_switch = obj;
                    lv_obj_set_pos(obj, 85, 0);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_INDICATOR | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
                {
                    // alarm_switch_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alarm_switch_label = obj;
                    lv_obj_set_pos(obj, 170, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "ON");
                }
                {
                    // setting_label_alarm
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.setting_label_alarm = obj;
                    lv_obj_set_pos(obj, -1, 4);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Alarm");
                }
                {
                    // max_temp_slider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.max_temp_slider = obj;
                    lv_obj_set_pos(obj, 5, 100);
                    lv_obj_set_size(obj, 150, 10);
                    lv_slider_set_range(obj, 0, 60);
                    lv_slider_set_value(obj, 33, LV_ANIM_OFF);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffe92d0f), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_opa(obj, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                }
                {
                    // max_temp_slider_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.max_temp_slider_label = obj;
                    lv_obj_set_pos(obj, -3, 56);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Maximum Temp C");
                }
                {
                    // max_diff_slider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.max_diff_slider = obj;
                    lv_obj_set_pos(obj, 5, 172);
                    lv_obj_set_size(obj, 150, 10);
                    lv_slider_set_range(obj, 0, 10);
                    lv_slider_set_value(obj, 4, LV_ANIM_OFF);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb6b6b6), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffe92d0f), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_opa(obj, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 2, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_KNOB | LV_STATE_PRESSED);
                    lv_obj_set_style_pad_bottom(obj, 5, LV_PART_KNOB | LV_STATE_PRESSED);
                    lv_obj_set_style_pad_left(obj, 5, LV_PART_KNOB | LV_STATE_PRESSED);
                    lv_obj_set_style_pad_right(obj, 5, LV_PART_KNOB | LV_STATE_PRESSED);
                }
                {
                    // max_diff_slider_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.max_diff_slider_label = obj;
                    lv_obj_set_pos(obj, -3, 130);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Maximum Difference C");
                }
                {
                    // max_temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.max_temp = obj;
                    lv_obj_set_pos(obj, 187, 56);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "33");
                }
                {
                    // max_diff
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.max_diff = obj;
                    lv_obj_set_pos(obj, 195, 161);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "4");
                }
            }
        }
        {
            // save_back
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.save_back = obj;
            lv_obj_set_pos(obj, 12, 280);
            lv_obj_set_size(obj, 40, 40);
            lv_image_set_src(obj, &img_minidisk);
            lv_image_set_scale(obj, 150);
            lv_obj_add_event_cb(obj, event_handler_cb_settings_save_back, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // back
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.back = obj;
            lv_obj_set_pos(obj, 190, 280);
            lv_obj_set_size(obj, 40, 40);
            lv_image_set_src(obj, &img_miniback);
            lv_image_set_scale(obj, 150);
            lv_obj_add_event_cb(obj, event_handler_cb_settings_back, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // probe_link
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.probe_link = obj;
            lv_obj_set_pos(obj, 103, 280);
            lv_obj_set_size(obj, 40, 40);
            lv_image_set_src(obj, &img_probe);
            lv_image_set_scale(obj, 255);
            lv_obj_add_event_cb(obj, event_handler_cb_settings_probe_link, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
}

void create_screen_probes() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.probes = obj;
    lv_obj_set_pos(obj, 1, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_add_event_cb(obj, event_handler_cb_probes_probes, LV_EVENT_ALL, flowState);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // my_probes_heading
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.my_probes_heading = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 33);
            add_style_style_normal(obj);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_line_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0e6411), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // probes_label_header
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.probes_label_header = obj;
            lv_obj_set_pos(obj, 39, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Temperature Probes");
        }
        {
            // settings_switch_panel_1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_switch_panel_1 = obj;
            lv_obj_set_pos(obj, 0, 39);
            lv_obj_set_size(obj, 240, 280);
            lv_obj_add_event_cb(obj, event_handler_cb_probes_settings_switch_panel_1, LV_EVENT_ALL, flowState);
            add_style_style_normal(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // probe_save_back
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.probe_save_back = obj;
            lv_obj_set_pos(obj, 12, 278);
            lv_obj_set_size(obj, 36, 40);
            lv_image_set_src(obj, &img_minidisk);
            lv_image_set_scale(obj, 150);
            lv_obj_add_event_cb(obj, event_handler_cb_probes_probe_save_back, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // probes_return
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.probes_return = obj;
            lv_obj_set_pos(obj, 190, 278);
            lv_obj_set_size(obj, 40, 40);
            lv_image_set_src(obj, &img_miniback);
            lv_image_set_scale(obj, 150);
            lv_obj_add_event_cb(obj, event_handler_cb_probes_probes_return, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 20, 61);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "#");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 83, 64);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Probe Address");
        }
        {
            // search_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.search_icon = obj;
            lv_obj_set_pos(obj, 106, 278);
            lv_obj_set_size(obj, 40, 40);
            lv_image_set_src(obj, &img_search);
            lv_image_set_scale(obj, 255);
            lv_obj_add_event_cb(obj, event_handler_cb_probes_search_icon, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // portal1
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.portal1 = obj;
            lv_obj_set_pos(obj, 5, 90);
            lv_obj_set_size(obj, 45, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "1\n2\n3\n4");
            lv_dropdown_set_selected(obj, 0);
        }
        {
            // portal4
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.portal4 = obj;
            lv_obj_set_pos(obj, 5, 225);
            lv_obj_set_size(obj, 45, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "1\n2\n3\n4");
            lv_dropdown_set_selected(obj, 3);
        }
        {
            // portal3
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.portal3 = obj;
            lv_obj_set_pos(obj, 5, 180);
            lv_obj_set_size(obj, 45, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "1\n2\n3\n4");
            lv_dropdown_set_selected(obj, 2);
        }
        {
            // portal2
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.portal2 = obj;
            lv_obj_set_pos(obj, 5, 135);
            lv_obj_set_size(obj, 45, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "1\n2\n3\n4");
            lv_dropdown_set_selected(obj, 1);
        }
        {
            // probes_container_sensor1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.probes_container_sensor1 = obj;
            lv_obj_set_pos(obj, 55, 90);
            lv_obj_set_size(obj, 175, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // probes_address_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.probes_address_1 = obj;
                    lv_obj_set_pos(obj, 10, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "## ## ## ## ## ## ## ##");
                }
            }
        }
        {
            // probes_container_sensor2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.probes_container_sensor2 = obj;
            lv_obj_set_pos(obj, 55, 135);
            lv_obj_set_size(obj, 175, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // probes_address_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.probes_address_2 = obj;
                    lv_obj_set_pos(obj, 10, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "## ## ## ## ## ## ## ##");
                }
            }
        }
        {
            // probes_container_sensor3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.probes_container_sensor3 = obj;
            lv_obj_set_pos(obj, 55, 180);
            lv_obj_set_size(obj, 175, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // probes_address_3
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.probes_address_3 = obj;
                    lv_obj_set_pos(obj, 10, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "## ## ## ## ## ## ## ##");
                }
            }
        }
        {
            // probes_container_sensor4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.probes_container_sensor4 = obj;
            lv_obj_set_pos(obj, 55, 225);
            lv_obj_set_size(obj, 175, 36);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_style_normal(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // probes_address_4
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.probes_address_4 = obj;
                    lv_obj_set_pos(obj, 10, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "## ## ## ## ## ## ## ##");
                }
            }
        }
    }
    
    tick_screen_probes();
}

void tick_screen_probes() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
}


extern void add_style(lv_obj_t *obj, int32_t styleIndex);
extern void remove_style(lv_obj_t *obj, int32_t styleIndex);

static const char *screen_names[] = { "Main", "Settings", "Probes" };
static const char *object_names[] = { "main", "settings", "probes", "settings_panel", "settings_switch_panel", "save_back", "back", "probe_link", "settings_switch_panel_1", "probe_save_back", "probes_return", "search_icon", "background", "banner_alert", "banner_alert_text", "front_left_container", "fl_portal_guage", "fl_portal_label", "front_right_container", "fr_portal_guage", "fr_portal_label", "rear_left_container", "rl_portal_guage", "rl_portal_label", "rear_right_container", "rr_portal_guage", "rr_portal_label", "my_horizontal_slider", "my_panel_header_1", "settings_label", "alarm_switch", "alarm_switch_label", "setting_label_alarm", "max_temp_slider", "max_temp_slider_label", "max_diff_slider", "max_diff_slider_label", "max_temp", "max_diff", "my_probes_heading", "probes_label_header", "obj0", "obj1", "portal1", "portal4", "portal3", "portal2", "probes_container_sensor1", "probes_address_1", "probes_container_sensor2", "probes_address_2", "probes_container_sensor3", "probes_address_3", "probes_container_sensor4", "probes_address_4" };
static const char *style_names[] = { "style_normal", "style_alarm" };


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_settings,
    tick_screen_probes,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    eez_flow_init_styles(add_style, remove_style);
    
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    eez_flow_init_style_names(style_names, sizeof(style_names) / sizeof(const char *));
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_settings();
    create_screen_probes();
}
