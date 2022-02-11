#pragma once

#include <Arduino.h>

#define NUM_ALARM 10

struct Alarm
{
  int hour;
  int min;
  int sec;
  uint8_t sound;
  int32_t time_int();
  void time_int(int32_t seconds);
};

class Config
{
public:
  Config();
  void setup();
  void store_wifi_settings(const String &name, const String &key);
  void store_clock_settings(const String &ntp_server_address, long utc_offset, int dst_offset);
  void store_alarm_settings(const Alarm& alarm, int i);
  const String &get_wifi_ssid();
  const String &get_wifi_psk();
  const String &get_ntp_server();
  long get_utc_offset_secs();
  int get_dst_offset_secs();
  Alarm get_alarm_settings(int i);

private:
  String ssid;
  String psk;
  String ntp_server;
  long utc_offset_secs;
  int dst_offset_secs;
  Alarm alarm[NUM_ALARM];
};

extern Config config;
