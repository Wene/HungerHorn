#pragma once

#include <Arduino.h>

class Lightshow
{
public:
  void setup();
  void tick(unsigned long now);
  void event();

private:
  unsigned long last_show;
  uint8_t step;
  uint32_t duration;
  uint32_t event_countdown;
};

extern Lightshow lightshow;
