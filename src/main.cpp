#include <Arduino.h>
#include "hid_core.h"

#define MOVE_STEP 15
#define SWIPE_STEP 10

bool is_keyboard_mode = false;

void serial_task(void *pvParameters) {
    for (;;) {
        if (Serial.available() > 0) {
            char c = Serial.read();
            
            if (c == '`') {
                is_keyboard_mode = !is_keyboard_mode;
                Serial.print("\n[SYSTEM] Mode changed to: ");
                Serial.println(is_keyboard_mode ? "KEYBOARD" : "MOUSE");
                continue;
            }
            
            if (c != '\n' && c != '\r') {
                if (is_keyboard_mode) {
                    hid_keyboard_type(c);
                } else {
                    if (c != ' ') {
                        switch (c) {
                            case 'w': hid_mouse_move(0, -MOVE_STEP); break;
                            case 's': hid_mouse_move(0, MOVE_STEP); break;
                            case 'a': hid_mouse_move(-MOVE_STEP, 0); break;
                            case 'd': hid_mouse_move(MOVE_STEP, 0); break;
                            case 'q': hid_mouse_click(); break;
                            case 'e': hid_mouse_toggle_hold(); break;
                            
                            case 'j': hid_mouse_swipe(-SWIPE_STEP, 0); break;
                            case 'l': hid_mouse_swipe(SWIPE_STEP, 0); break;
                            case 'i': hid_mouse_swipe(0, -SWIPE_STEP); break;
                            case 'k': hid_mouse_swipe(0, SWIPE_STEP); break;
                            
                            case 'x': hid_macro_toggle_bluetooth(); break;
                        }
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    hid_init();
    
    xTaskCreatePinnedToCore(
        serial_task,
        "SerialTask",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}