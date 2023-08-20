#include <Encoder.h>

#include "state.h"
#include "variable.h"
#include "esp-now.h"
#include "interval.h"
#include "position.h"

#define ENCODER_A 35
#define ENCODER_B 34

// Second core and x, v encoder ///////////////////////
void loop2(void * params) {
  Encoder encoder(ENCODER_A, ENCODER_B); //todo possibly edit code to add esp32 attribute needed for interrupts

  int lastStep = steps % 2400;
  for (;;) {
    steps = encoder.read();
  }
}
void setup2() {
  TaskHandle_t core2;

  xTaskCreatePinnedToCore(
      loop2, /* Function to implement the task */
      "DAQ", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &core2,  /* Task handle. */
      0); /* Core where the task should run */ //??
}

Position position(state);

// First core //////////////////////////////////////////
void setup() {
  // Setup second core
  setup2();

  // Setup motor
  // channel, freq, resolution(bits)
  ledcSetup(RPWM_CHANNEL, 500, 8);
  ledcSetup(LPWM_CHANNEL, 500, 8);
  ledcAttachPin(RPWM_OUT, RPWM_CHANNEL);
  ledcAttachPin(LPWM_OUT, LPWM_CHANNEL);

  // Initialize communications
  Serial.begin(115200);
  state.init();
  setupEspNow();
}

void loop() {
  position.loop();
  state.loop();
}
