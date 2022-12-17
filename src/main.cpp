#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include "./Protocol/BlueWireTask.h"

TaskHandle_t handleBlueWireTask;

void setup() {
  // create task to run blue wire interface
  xTaskCreate(BlueWireTask,
    "BlueWireTask",
    1600,
    NULL,
    4,
    &handleBlueWireTask);

}

void loop() {
}
