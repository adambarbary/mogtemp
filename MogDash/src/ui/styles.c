#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: style_normal
//

void init_style_style_normal_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff0e6411));
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_opa(style, 255);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_style_normal_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_normal_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_normal(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_normal_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_normal(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_normal_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: style_alarm
//

void init_style_style_alarm_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffff0000));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_opa(style, 255);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_style_alarm_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_alarm_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_alarm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_alarm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_alarm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_alarm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_style_normal,
        add_style_style_alarm,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_style_normal,
        remove_style_style_alarm,
    };
    remove_style_funcs[styleIndex](obj);
}

