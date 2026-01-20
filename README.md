# mogtemp
Multipoint temperature sensor and display designed for Unimog portal monitoring
Code is designed to work on ESP32 CYD (cheap yellow display) using EEZ Studio and Arduino IDE

Thanks to these projects for inspiration and examples
EEZ Studio - https://www.envox.eu/studio/studio-introduction/
Arduino IDE - https://www.arduino.cc/en/software
Original temp sensing code from https://turnleftatcairo.weebly.com/code-for-the-arduino.html
based on: http://swingleydev.org/blog/tag/arduino/ (no longer available)
Interface design, EEZ LVGL tutorial & inspiration from Uteh Str - https://www.youtube.com/watch?v=4k013Crfce8 - https://drive.google.com/file/d/1s4dtXSGeFvR-TereeB9-4iUszsPacVj_/view
Help with setup and settings by Rui Santos & Sara Santos-Random Nerd Tutorials https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/

Hardware - Not exhaustive, just what I have used so far
ESP32 - CYD ESP32-2432S028 - Note, this board didn't carry the (R) designation
Onewire temp sensors - Connected via Pin 27
