#pragma once

#include <Arduino.h>

class Network
{
public:
  void setup();
  void config_start();
  void tick(unsigned long now);

private:
  void scan(const String &input);
  void select(const String &input);
  void password(const String &input);
  void search();
  void connect();
  int num_net;
  int slot;
  String ssid;
  String psk;
  unsigned long last_tick;
  uint32_t connecting_countdown;
};

extern Network network;
