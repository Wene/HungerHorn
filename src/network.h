#pragma once

#include <Arduino.h>

class Network
{
public:
  void setup();
  void config_start();

private:
  void scan(const String &input);
  void select(const String &input);
  void password(const String &input);
  void connect();
  int num_net;
  int slot;
  String ssid;
  String psk;
};

extern Network network;
