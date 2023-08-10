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
  Serial.println("culo");
}

float x, v;
void applyControl();
void loop() {
  // todo make better
  static int lastMicros;
  static int lastSteps;
  static int deltaTime;

  deltaTime = micros() - lastMicros;


  if (deltaTime > 5000) {
    constexpr float toMeters = .853 / 51144.;

    x = steps * toMeters;
    v = ((steps - lastSteps) * 1E6) / deltaTime * toMeters;
    lastMicros = micros();
    lastSteps = steps;

    Serial.print(x);
    Serial.print(" ");
    Serial.print(v);
    Serial.println();
  }

    applyControl();
}

enum State {
    positiveRunning,
    positiveBack,
    positiveWaitBeforeRunning,
    positiveWaitBeforeBack,
    negativeRunning,
    negativeBack,
    negativeWaitBeforeRunning,
    negativeWaitBeforeBack,
    switching,
    stop
};

int step = 1;
int stepSize = 32;
State state = positiveWaitBeforeRunning;
void control(int ctrl) {
    ledcWrite(LPWM_CHANNEL, ctrl > 0 ?  ctrl : 0);
    ledcWrite(RPWM_CHANNEL, ctrl < 0 ? -ctrl : 0);
}
void applyControl() {
    switch (state) {
        case positiveRunning:
            //write to console
            if (x > .5) {
                state = positiveWaitBeforeBack;
            }
            break;
            
        case positiveBack:
            control(-50);
            if (x <= 0) {
                state = positiveWaitBeforeRunning;
            }
            break;

        case positiveWaitBeforeBack:
            control(-20);
            delay(1000);
            state = positiveBack;
            break;

        case positiveWaitBeforeRunning:
            control(0);
            delay(1000);
            step += 1;
            if (step * stepSize > 128) {
                state = switching;
                break;
            }

            control(step * stepSize);
            state = positiveRunning;
            break;

        case negativeRunning:
            //write to console
            if (x < .3) {
                state = negativeWaitBeforeBack;
            }
            break;

        case negativeWaitBeforeBack:
            control(20);
            delay(1000);
            state = negativeBack;
            break;

        case negativeWaitBeforeRunning:
            control(0);
            delay(1000);
            step -= 1;
            if (step * stepSize < -128) {
                state = stop;
                control(0);
                break;
            }

            control(step * stepSize);
            state = negativeRunning;
            break;

        case negativeBack:
            control(50);
            if (x >= .8) {
                state = negativeWaitBeforeRunning;
            }
            break;


        case switching:
            step = -1;
            control(50);
            if (x >= .8) {
                state = negativeWaitBeforeRunning;
            }
            break;

        default:
          break;
    }

}