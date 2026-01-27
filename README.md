# mogtemp
Multipoint temperature sensor and display designed for Unimog portal monitoring
Code is designed to work on ESP32 CYD (cheap yellow display) using EEZ Studio and Arduino IDE

*Note - complete code can be compiled from Mog_Portal_X, MogDashVX contains interface designs, for those who wish to modify the look and feel. 
If compiling a new dash, and you get a compile error, please update the following two lines of code noted at the end of this readme. 

Thanks to these projects for inspiration and examples
EEZ Studio - https://www.envox.eu/studio/studio-introduction/
Arduino IDE - https://www.arduino.cc/en/software
Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
based on: http://swingleydev.org/blog/tag/arduino/ (no longer available)
Interface design concept, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8 - https://drive.google.com/file/d/1s4dtXSGeFvR-TereeB9-4iUszsPacVj_/view
Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/
Save icon created by Yogi Aprelliyanto - Flaticon https://www.flaticon.com/free-icons/save
Return icon created by Kiranshastry - Flaticon https://www.flaticon.com/free-icons/return

Hardware - Not exhaustive, just what I have used so far
ESP32 - CYD ESP32-2432S028 - Note, this board didn't carry the (R) designation
Onewire temp sensors - Connected via Pin 27

Flow update includes dynamic diff and max temp settings, as well as alarm switch. 
Temps are read and updated every 4 seconds (can be altered by changing TEMP_READ_INTERVAL)

// To come(well, the plan is...)
Save/Load to SD card
Warning LED behaviour to change to onscreen warning (colour change, or pop-up, or something)
Mute for alarm
Change to Speaker connector for alarm


If you update the dash, and you start to receive compile errors, change the following.
In eez-flow.cpp, change:
lv_obj_get_style_opa((lv_obj_t *)a->user_data, 0); }
To:
lv_obj_get_style_opa((lv_obj_t *)a->user_data, LV_PART_MAIN); }
AND
int32_t opa = (int32_t)lv_obj_get_style_opa(obj, 0);
To:
int32_t opa = (int32_t)lv_obj_get_style_opa(obj, LV_PART_MAIN);
