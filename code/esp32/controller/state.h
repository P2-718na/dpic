#ifndef STATE_H
#define STATE_H
#include <Arduino.h>
#include "interval.h"

// Constants
#define RPWM_OUT 33
#define LPWM_OUT 32
#define RPWM_CHANNEL 0
#define LPWM_CHANNEL 1



template <typename T> inline int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

struct State {
  float x, v, a1, w1, a2, w2;
  float currentControl;
  Interval printInterval;
  Interval controlInterval;

  State();
  void init();
  float optimalControl();
  bool  controllable();
  void  updateControl();
  void  updateMotor();
  int   convertToPWM(float control);
  void  print();
  void  loop();
};

#endif