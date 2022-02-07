#pragma once

#include <Arduino.h>

class Network
{
public:
  void setup();
  void scan();

private:
  void select(const String &input);
  void password(const String &input);
  void connect();
  int num_net;
  String ssid;
  String psk;
};

extern Network network;
