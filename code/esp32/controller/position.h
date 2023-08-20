#ifndef DPIC_ENCODER_H
#define DPIC_ENCODER_H
#include <Ewma.h>
#include "state.h"
#include "interval.h"

// This is shared between cores. I'm gonna keep this out of the struct because I don't know what I'm doing.
extern volatile int steps;

struct Position {
  static constexpr float toMeters = .853 / 51144.;
  Ewma filter;
  State& state;
  Interval interval;
  int lastSteps;

  Position(State& state);

  void loop();
};

#endif //DPIC_ENCODER_H
