#include "hid_core.h"
#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;
bool is_holding = false;

void hid_init() {
    Mouse.begin();
    Keyboard.begin();
    USB.begin();
}

void hid_mouse_move(int8_t x, int8_t y) {
    Mouse.move(x, y);
}

void hid_mouse_click() {
    Mouse.click();
}

void hid_mouse_toggle_hold() {
    if (is_holding) {
        Mouse.release();
        is_holding = false;
    } else {
        Mouse.press();
        is_holding = true;
    }
}

void hid_mouse_swipe(int8_t dx, int8_t dy) {
    Mouse.press();
    delay(30);
    
    for (int i = 0; i < 15; i++) {
        Mouse.move(dx, dy);
        delay(10);
    }
    
    Mouse.release();
}

void hid_keyboard_type(char c) {
    Keyboard.write(c);
}

void hid_macro_toggle_bluetooth() {
    for (int i = 0; i < 20; i++) {
        Mouse.move(-127, -127);
        delay(10);
    }
    delay(100);
    Mouse.move(127,127);
    delay(20);
    Mouse.move(20,60);
    delay(100);
    Mouse.press();
    delay(50);
    for (int i = 0; i < 15; i++) {
        Mouse.move(0, 25);
        delay(10);
    }
    Mouse.release();
    delay(200);
    Keyboard.print("bluetooth");
    delay(200);
    for(int i = 0;i < 3; ++i){
        Mouse.move(30,-95);
        delay(10);
    }
    delay(500);
    Mouse.click();
    delay(500);
    for (int i = 0; i < 20; i++) {
        Mouse.move(-5, 127);
        delay(10);
    }
    Mouse.press();
    delay(50);
    for (int i = 0; i < 10; i++) {
        Mouse.move(0, -25);
        delay(10);
    }
    Mouse.release();
}