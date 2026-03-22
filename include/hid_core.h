#ifndef HID_CORE_H
#define HID_CORE_H

#include <Arduino.h>

void hid_init();
void hid_mouse_move(int8_t x, int8_t y);
void hid_mouse_click();
void hid_mouse_toggle_hold();
void hid_mouse_swipe(int8_t dx, int8_t dy);
void hid_keyboard_type(char c);
void hid_macro_toggle_bluetooth();

#endif