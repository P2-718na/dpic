#include "interval.h"
#include <Arduino.h>

Interval::Interval(int interval) : interval(interval), lastTime(millis()) {}

bool Interval::ready() {
    return millis() - lastTime >= interval;
}

void Interval::reset() {
  lastTime = millis();
}