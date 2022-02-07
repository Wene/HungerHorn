#pragma once

#include <Arduino.h>
#include <time.h>

class AlarmClock
{
public:
  void setup();
  void printLocalTime();
  void tick();

private:
  struct tm timeinfo;
};


extern AlarmClock alarmclock;
