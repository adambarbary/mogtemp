#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_ALARM_PANEL = 0
};

// Native global variables
extern char alarm_panel[100];                          // ← add this
const char *get_var_alarm_panel();                     // ← add this
void set_var_alarm_panel(const char *value);           // ← add this


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/