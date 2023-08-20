#include <esp_now.h>
#include <WiFi.h>
#include <Encoder.h>


// Constants 
#define RPWM_OUT 32
#define LPWM_OUT 33
#define RPWM_CHANNEL 0
#define LPWM_CHANNEL 1

#define ENCODER_A 35
#define ENCODER_B 34


// State  and control /////////////////////////////////////////////
struct State {
  float x, v, a1, w1, a2, w2;

  float optimalControl() {
    //return 2*cos(millis()/200.);
    return 1;
  }

  bool controllable() {
    return x < .7  && x > .01;
  }
};
State state;
void printState();


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
  }

  printState(state);

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
  if (millis() - interval < 100) {
    return;
  }

  Serial.print(state.x);
  Serial.print(" ");
  Serial.print(state.v);
  Serial.print(" ");
  Serial.println();

  interval = millis();
}

void applyControl(State& state) {
  static int interval = millis();
  if (millis() - interval < 20) {
    return;
  }


  float control = state.optimalControl();

  // QUi ci andrà uno switch per tutti i possibili stati del sistema (down, up, swing up...)
  if (state.controllable()) {
    //Serial.print("+");
    driveMotorWithForce(control, state.v);
    return;
  }

  //Serial.print("=");
  driveMotorWithForce(0, 0);
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// PEso motore: 225g senza filo, 240g con filo.
void driveMotorWithForce(float f, float v) {
    constexpr float A =  .656;
    constexpr float B =  .0616;
    constexpr float C = -.0535;

    // deltaT
    constexpr float dT = .02;

    //m
    constexpr float m = .8;

    float u = 
      ((1 - A)*v + dT*f/m - C*sgn(v))
      / 
      B;
    u = u *255./12.;

    if (f == 0) {
      ledcWrite(LPWM_CHANNEL, 0);
      ledcWrite(RPWM_CHANNEL, 0);

      return;
    }

    ledcWrite(LPWM_CHANNEL, (int)(u > 0 ?  u : 0));
    ledcWrite(RPWM_CHANNEL, (int)(u < 0 ? -u : 0));
}
