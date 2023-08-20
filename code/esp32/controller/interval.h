#ifndef DPIC_TIMER_H
#define DPIC_TIMER_H

struct Interval {
  const int interval;
  int lastTime;

  // interval (millis)
  Interval(int interval);
  bool ready();
  void reset();
};

#endif //DPIC_TIMER_H
