#pragma once

#include <Arduino.h>

struct TimeConfig
{
  int hour;
  int min;
  int sec;
};

class Config
{
public:
  Config();
  void setup();
  void store_wifi_settings(const String &name, const String &key);
  void store_clock_settings(const String &ntp_server_address, long utc_offset, int dst_offset);
  void store_alarm_settings(const TimeConfig& alarm);
  const String &get_wifi_ssid();
  const String &get_wifi_psk();
  const String &get_ntp_server();
  long get_utc_offset_secs();
  int get_dst_offset_secs();
  TimeConfig get_alarm_settings();

private:
  String ssid;
  String psk;
  String ntp_server;
  long utc_offset_secs;
  int dst_offset_secs;
  TimeConfig alarm_time;
};

extern Config config;
