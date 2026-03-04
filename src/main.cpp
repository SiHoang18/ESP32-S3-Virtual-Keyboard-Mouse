#include <Arduino.h>
#include "../src/Hid/hid.h"
#include "../src/Webserver/localhost.h"

void setup() {
    cmdQueue = xQueueCreate(20, sizeof(HIDCommand));
    if (cmdQueue != NULL) {
        xTaskCreatePinnedToCore(webTask, "WebTask", 4096, NULL, 1, NULL, 0);
        xTaskCreatePinnedToCore(hidTask, "HIDTask", 4096, NULL, 2, NULL, 1);
    }
}

void loop() {
    vTaskDelete(NULL);
}