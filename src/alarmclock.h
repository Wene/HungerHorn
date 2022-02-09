#pragma once

#include <Arduino.h>

class AlarmClock
{
public:
  void setup();
  void tick();
  void config_start();
  void print_time();

private:
  void config_ntp(const String& input);
  void config_utc(const String& input);
  void config_dst(const String& input);
  String ntp_server;
  long utc_offset;
  int dst_offset;
  unsigned long last_print;
};


extern AlarmClock alarmclock;
