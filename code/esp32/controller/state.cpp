#include "state.h"

State::State() : printInterval(Interval(100)), controlInterval(Interval(10)), currentControl(0) {}

void State::init() {
  Serial.println("x v a1 w1 a2 w2");
}

float State::optimalControl() {
 // return 1;
  return (3*x + 3*v + 16*a1 + 1*w1)*.8;
}

bool State::controllable() {
  //return x <.6 && x >.01;
  return (a1 > -.4  && a1 < .4 && x < .35 && x > -.35);
}

void State::updateControl() {
  // Qui ci va lo switch in base allo stato del sistema...
  if (!controllable()) {
    currentControl = 0;
    return;
  }

  Serial.print(millis());
  Serial.println("+");
  currentControl = optimalControl();
}

int State::convertToPWM(float force) {
  constexpr float A = 1.8;
  constexpr float B = 6; // Dovrebbe essere 8, ma 8 non va lol

  float u = A*force + B*v;
  u *= 255/14.;

  return u;
}

void State::updateMotor() {
  if (currentControl == 0) {
    ledcWrite(LPWM_CHANNEL, 0);
    ledcWrite(RPWM_CHANNEL, 0);

    return;
  }

  int u = convertToPWM(currentControl);

  constexpr int LPWM_MIN = 18;
  constexpr int RPWM_MIN = 18;

  ledcWrite(LPWM_CHANNEL, (int)(u > 0 ?  u + LPWM_MIN: 0));
  ledcWrite(RPWM_CHANNEL, (int)(u < 0 ? -u + RPWM_MIN: 0));
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