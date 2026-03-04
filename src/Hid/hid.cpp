#include "hid.h"
#include <USB.h>
#include <USBHIDKeyboard.h>

USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
QueueHandle_t cmdQueue;

void hidTask(void *pvParameters) {
    Keyboard.begin();
    Mouse.begin();
    USB.begin();

    HIDCommand cmd;
    for (;;) {
        if (xQueueReceive(cmdQueue, &cmd, portMAX_DELAY)) {
            if (cmd.type == CMD_SWIPE) {
                Mouse.press(MOUSE_LEFT);
                vTaskDelay(pdMS_TO_TICKS(50));
                
                int steps = cmd.duration;
                int dx = cmd.x / steps;
                int dy = cmd.y / steps;
                
                for (int i = 0; i < steps; i++) {
                    Mouse.move(dx, dy);
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                
                vTaskDelay(pdMS_TO_TICKS(50));
                Mouse.release(MOUSE_LEFT);
                
            } else if (cmd.type == CMD_LONG_PRESS) {
                Mouse.press(cmd.button);
                vTaskDelay(pdMS_TO_TICKS(cmd.duration));
                Mouse.release(cmd.button);
            } else if (cmd.type == CMD_MOUSE_MOVE) {
                Mouse.move(cmd.x, cmd.y);
            } else if (cmd.type == CMD_MOUSE_CLICK) {
                Mouse.click(cmd.button);
            }
        }
    }
}