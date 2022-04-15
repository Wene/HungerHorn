#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

#define NUM_ALARM 10

class Preferences;

struct Alarm
{
  Alarm(): secs_in_day(-1) {};
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
  void add_event_callback(std::function<void(void)> event);

private:
  String ntp_server;
  String tz_str;
  Preferences *settings;

  Alarm alarm[NUM_ALARM];
  Alarm setup_alarm;

  std::vector<std::function<void(void)>> event_callbacks;

  void alarm_setup_no(const String& input);
  void alarm_setup_act(const String& input);
  void alarm_setup_snd(const String& input);
  void alarm_setup_time(const String& input);
  void store_setup_alarm();
  void setup_time();

  void config_ntp(const String& input);
  void config_tz(const String& input);

  int setup_index;

  unsigned long last_update;
};


extern AlarmClock alarmclock;
