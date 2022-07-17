#pragma once

#include <Arduino.h>
#include <vector>

class Preferences;

struct WifiConfig
{
  String ssid;
  String psk;
};

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
  bool setup_active;
  unsigned long config_slot;
  unsigned long last_tick;
  int num_net;
  uint32_t connecting_countdown;
  std::vector<WifiConfig> wifi_list;
  WifiConfig active_config;
};

extern Network network;
