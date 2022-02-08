#pragma once

#include <Arduino.h>
#include <time.h>

class AlarmClock
{
public:
  void setup();
  void printLocalTime();
  void tick();
  void config_start();

private:
  void config_ntp(const String& input);
  void config_utc(const String& input);
  void config_dst(const String& input);
  struct tm timeinfo;
  String ntp_server;
  long utc_offset;
  int dst_offset;
};


extern AlarmClock alarmclock;
