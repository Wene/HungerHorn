#pragma once

#include <Arduino.h>

class Config
{
public:
  Config();
  void setup();
  void store_wifi_settings(const String &name, const String &key);
  const String &get_wifi_ssid();
  const String &get_wifi_psk();

private:
  String ssid;
  String psk;
};

