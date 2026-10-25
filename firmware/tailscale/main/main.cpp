#include <Arduino.h>
#include "controller.h"

static void controllerTask(void*) {
  setup();
  for (;;) loop();
}

extern "C" void app_main() {
  initArduino();
  // Preserve the controller loop on core 1; networking runs independently.
  if (xTaskCreatePinnedToCore(controllerTask, "ito-control", 8192, nullptr,
                            2, nullptr, 1) != pdPASS) {
    // setup() never ran: explicitly keep all three power enables low.
    pinMode(D3, OUTPUT); digitalWrite(D3, LOW);
    pinMode(D6, OUTPUT); digitalWrite(D6, LOW);
    pinMode(D9, OUTPUT); digitalWrite(D9, LOW);
  }
}
