#include <WiFi.h>
#include <esp_now.h>
#include <Arduino.h>
#include "esp-now.h"
#include "variable.h"

State state;

void OnDataRecv(const uint8_t * maxsc, const uint8_t *incomingData, int len)  {
  // ESP-NOW Data structure
  static Variable data;
  memcpy(&data, incomingData, sizeof(data));

  if (data.id == 1) {
    state.a1 = data.q;
    state.w1 = data.q_dot;

    return;
  }

  state.a2 = data.q;
  state.w2 = data.q_dot;
}

void setupEspNow()  {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    // Todo handle errors
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

