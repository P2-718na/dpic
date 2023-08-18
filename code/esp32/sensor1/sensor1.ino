#include <esp_now.h>
#include <WiFi.h>
#include <Encoder.h>
#include <Ewma.h>
//EwmaT <int> speedFilter(33, 100);

#define ENCODER_A 35
#define ENCODER_B 34
#define BROADCAST_ADDRESS {0xBC, 0xDD, 0xC2, 0xD4, 0x52, 0x28}

struct Variable {
    byte id;
    float q;
    float q_dot;
};
Variable data;
uint8_t broadcastAddress[] = BROADCAST_ADDRESS;
void setupEspNow();



volatile int steps;
int euclideanMod(int a, int b);
void loop2(void * params) {
  Encoder encoder(ENCODER_A, ENCODER_B); //todo possibly edit code to add esp32 attribute needed for interrupts

  for (;;) {
    // maybe I cann add some sort of timer here...
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



void setup() {
  // Setup second core
  setup2();
  // For some reason, initializing serial here and not writing to console breaks esp now... ?
  //Serial.begin(115200);

  setupEspNow();
}

void loop() {
  static int lastMicros = 0;
  static int deltaTime;
  static int lastSteps;
  deltaTime = micros() - lastMicros;

  //if (deltaTime1 > 1001) {
  //  speedFilter.filter((steps - lastSteps) * 1E6 / deltaTime1);
  //  lastSteps = steps;
  //  lastMicros1 = micros();
  //}
  
  // 5ms delay between sends
  if (deltaTime > 2000) {
    constexpr float toRadians = M_PI / 1200.;

    // Just to be sure, I want to avoid race conditions.
    const int currentSteps = steps;

    static Ewma q_dotFilter(.1);

    data.id     = 1;
    data.q      = (float)(euclideanMod(currentSteps, 2400) - 1200) * toRadians ;
    data.q_dot  = q_dotFilter.filter(
      (float)(currentSteps - lastSteps) / (float)deltaTime * toRadians * 1E6
    );
  
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));

    lastSteps = currentSteps;
    lastMicros = micros();
  }
}



void setupEspNow() {
  // Master esp32
  esp_now_peer_info_t peerInfo;

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

int euclideanMod(int a, int b) {
  int mod = a % b;
  return mod > 0 ? mod : mod + b;
}