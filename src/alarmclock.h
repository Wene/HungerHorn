#pragma once

#include <Arduino.h>
#include "config.h"

class AlarmClock
{
public:
  void setup();
  void tick(unsigned long now);
  void config_start();
  void alarm_setup_start();
  void print_time();

private:
  Alarm setup_alarm;
  int setup_index;
  void alarm_setup_no(const String& input);
  void alarm_setup_act(const String& input);
  void alarm_setup_snd(const String& input);
  void alarm_setup_time(const String& input);

  void config_ntp(const String& input);
  void config_utc(const String& input);
  void config_dst(const String& input);

  String ntp_server;
  long utc_offset;
  int dst_offset;

  unsigned long last_update;
};


extern AlarmClock alarmclock;
