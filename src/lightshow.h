#pragma once

#include <Arduino.h>

class Lightshow
{
public:
  void setup();
  void tick(unsigned long now);

private:
  unsigned long last_show;
  uint8_t step;
  uint32_t duration;
};

