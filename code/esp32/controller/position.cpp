#include "position.h"

volatile int steps;

Position::Position(State &state) :
  state(state),
  interval(Interval(2)),
  filter(Ewma(.1))
{}

void Position::loop() {
  static int lastMicros;

  if (!interval.ready()) {
    return;
  }

  float deltaTime = micros() - lastMicros;

  state.x = steps * toMeters;
  state.v = filter.filter(
      ((steps - lastSteps) * 1E6) / deltaTime * toMeters
  );
  lastSteps = steps;

  interval.reset();
  lastMicros = micros();
}