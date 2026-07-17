#include <string.h>
#include "vars.h"

char alarm_panel[100] = { 0 };

const char *get_var_alarm_panel() {
    return alarm_panel;
}

void set_var_alarm_panel(const char *value) {
    strncpy(alarm_panel, value, sizeof(alarm_panel) - 1);
    alarm_panel[sizeof(alarm_panel) - 1] = 0;
}
