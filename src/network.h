#pragma once

#include <Arduino.h>

#define NUM_WIFI 5

class Preferences;

class Network
{
public:
  Network();
  ~Network();
  void setup();
  void config_start();
  void tick(unsigned long now);

private:
  Preferences *settings;
  void scan(const String &input);
  void select(const String &input);
  void password(const String &input);
  void search();
  void connect();
  int num_net;
  int slot;
  bool setup_active;
  String active_ssid;
  String active_psk;
  unsigned long last_tick;
  uint32_t connecting_countdown;
  String ssid_list[NUM_WIFI];
  String psk_list[NUM_WIFI];
};

extern Network network;
