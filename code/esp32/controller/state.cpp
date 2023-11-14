#include "state.h"
#include "math.h"

State::State() : printInterval(Interval(25)), controlInterval(Interval(5)), currentControl(0) {}

void State::init() {
  Serial.println("x v a1 w1 a2 w2");
}

float State::optimalControl() {
 // return 1;
 // SIngle pendulum
  return (3.63*x + 2.77*v + 9.89*a1 + 1.18*w1);
  return -(1.8*x + 2.9*v - 30*a1 - 0.97*w1 + 80.5*a2 + 8.00*w2)*.1;
}

bool State::controllable() {
  //return x <.6 && x >.01;
  return (a1 > -.8  && a1 < .8 && x < .37 && x > -.37);
  return (a1 > -.8  && a1 < .8 && x < .35 && x > -.35 && a2 > -.5  && a2 < .5);
}

void State::updateControl() {
  // Qui ci va lo switch in base allo stato del sistema...
  if (!controllable()) {
    currentControl = 0;

    constexpr float aMax = 10;
    cosnt float ctrl = tanh(
        cos(a1) * w1 (
                      0.0243628 * (-1 + cos(a1)) +
                      0.00018 * w1*w1
                      )
    )

    if (abs(a1) > 1 && x < .3 && x > -.3 && abs(v) < 2) {
      currentControl = ctrl;
    }

    return;
  }

  Serial.print(millis());
  Serial.println("+");
  currentControl = optimalControl();
}

int State::convertToPWM(float force) {
  constexpr float A = 28.5;
  constexpr float B = 120 ; // Dovrebbe essere 8, ma 8 non va lol

  float u = A*force + B*v;

  return u;
}

void State::updateMotor() {
  if (currentControl == 0) {
    ledcWrite(LPWM_CHANNEL, 0);
    ledcWrite(RPWM_CHANNEL, 0);

    return;
  }

  int u = convertToPWM(currentControl);

  constexpr int LPWM_MIN = 33; //32 safe
  constexpr int RPWM_MIN = 33; // 32 safe

  ledcWrite(LPWM_CHANNEL, (int)(u > 0 ?  (u < LPWM_MIN ? u + LPWM_MIN : u): 0));
  ledcWrite(RPWM_CHANNEL, (int)(u < 0 ?  (-u < RPWM_MIN ? -u + RPWM_MIN : -u) : 0));
}

void State::print() {
  Serial.print("( ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(v);
  Serial.print(", ");
  Serial.print(a1);
  Serial.print(", ");
  Serial.print(w1);
  Serial.print(", ");
  Serial.print(a2);
  Serial.print(", ");
  Serial.print(w2);
  Serial.print(", ");
  Serial.print(optimalControl());
  Serial.print(", ");
  Serial.print(convertToPWM(optimalControl()));
  Serial.print(", ");
  Serial.print((int)controllable());
  Serial.print(" )");
  Serial.println();
}

void State::loop() {
  if (printInterval.ready()) {
    print();
    printInterval.reset();
  }

  updateMotor();

  if (controlInterval.ready()) {
    updateControl();
    controlInterval.reset();
  }
}