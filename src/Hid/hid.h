#ifndef HID_H
#define HID_H

#include <Arduino.h>
#include <USBHIDMouse.h>

enum CmdType { CMD_KEYBOARD, CMD_MOUSE_MOVE, CMD_MOUSE_CLICK, CMD_SWIPE, CMD_LONG_PRESS };

struct HIDCommand {
    CmdType type;
    char key;
    int x;
    int y;
    uint8_t button;
    int duration;
};

extern QueueHandle_t cmdQueue;

void hidTask(void *pvParameters);

#endif