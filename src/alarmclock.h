#pragma once

#include <Arduino.h>

#define NUM_ALARM 10

class Preferences;

struct Alarm
{
  int32_t secs_in_day;
  uint8_t sound;
  int hour();
  int min();
  int sec();
  void hour(int new_val);
  void min(int new_val);
  void sec(int new_val);
};

class AlarmClock
{
public:
  AlarmClock();
  ~AlarmClock();

  void setup();
  void tick(unsigned long now);
  void config_start();
  void alarm_setup_start();
  void print_time();

private:
  String ntp_server;
  long utc_offset;
  int dst_offset;
  Preferences *settings;

  Alarm alarm[NUM_ALARM];
  Alarm setup_alarm;

  void alarm_setup_no(const String& input);
  void alarm_setup_act(const String& input);
  void alarm_setup_snd(const String& input);
  void alarm_setup_time(const String& input);
  void store_setup_alarm();

  void config_ntp(const String& input);
  void config_utc(const String& input);
  void config_dst(const String& input);

  int setup_index;

  unsigned long last_update;
};


extern AlarmClock alarmclock;
