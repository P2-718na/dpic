#include <esp_now.h>
#include <WiFi.h>
#include <Encoder.h>
#include <EwmaT.h>


// Constants 
#define RPWM_OUT 32
#define LPWM_OUT 33
#define RPWM_CHANNEL 0
#define LPWM_CHANNEL 1

#define ENCODER_A 34
#define ENCODER_B 35


// State  and control /////////////////////////////////////////////
struct State {
  float x, v, a1, w1, a2, w2;

  // Assume control setpoint to be at zero.
  // NOTE THST THIS SHOULD BE INTEGER KYS ARDUINO
  int optimalControl() {
    return (a1 > 0 ? 50 : -50) + 25.*a1 + 20.*w1;//-50;//-x/60.; //+ v;//400*a1 + 150*w1;
  }

  bool controllable() {
    return (a1 > -.6 && a1 < -.001) || (a1 > .001  && a1 < .6);
  }
};
State state;
void printState();



// ESP-NOW Data structure /////////////////////////////
struct Variable {
  byte id;
  float q;
  float q_dot;
};
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
void setupEspNow();



// Second core and x, v encoder ///////////////////////
volatile int steps;
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
  setupEspNow();
}

void loop() {

  // todo make better
  static int lastMicros;
  static int lastSteps;
  static int deltaTime;

  deltaTime = micros() - lastMicros;
  if (deltaTime > 5000) {
    constexpr float toMeters = .853 / 51144.;

    state.x = steps * toMeters;
    state.v = ((steps - lastSteps) * 1E6) / deltaTime * toMeters;
    lastMicros = micros();
    lastSteps = steps;


    printState(state);
    //if (state.controllable()) {
    //  Serial.print(millis());
    //  Serial.print(" ");
    //  Serial.print(state.v);
    //  Serial.print("\n");
    //}
  }


  

  applyControl(state);
}



// Function definitions /////////////////////////////////////
void printState(State& state) {
  static bool init = false;
  if (!init) {
    Serial.println("x v a1 w1 a2 w2");

    init = true;
  }

  static int interval = millis();
  if (millis() - interval < 50) {
    return;
  }

  Serial.print(state.x);
  Serial.print(" ");
  Serial.print(state.v);
  Serial.print(" ");
  Serial.print(state.a1);
  Serial.print(" ");
  Serial.print(state.w1);
  Serial.print(" ");
  Serial.print(state.a2);
  Serial.print(" ");
  Serial.print(state.w2);
  Serial.print(" ");
  Serial.println();

  interval = millis();
}

void applyControl(State& state) {
  int control = state.optimalControl();

  // QUi ci andrà uno switch per tutti i possibili stati del sistema (down, up, swing up...)
  if (state.controllable()) {
    Serial.print("+");
    //Fixme analogwrite does not work on esp32
    ledcWrite(LPWM_CHANNEL, (int)(control > 0 ?  control : 0));
    ledcWrite(RPWM_CHANNEL, (int)(control < 0 ? -control : 0));
    return;
  }

  //Serial.print("=");
  ledcWrite(LPWM_CHANNEL, 0);
  ledcWrite(RPWM_CHANNEL, 0);
}


void OnDataRecv(const uint8_t * maxsc, const uint8_t *incomingData, int len) {
  // ESP-NOW Data structure
  static Variable data;
  memcpy(&data, incomingData, sizeof(data));

  // Todo change ids to something that makes sense
  if (data.id == 1) {
    state.a1 = data.q;
    state.w1 = data.q_dot;
    
    return;
  }

  state.a2 = data.q;
  state.w2 = data.q_dot;  
}

void setupEspNow() {
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