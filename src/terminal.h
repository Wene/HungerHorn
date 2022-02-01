#pragma once

#include <Arduino.h>

typedef void (*tCB_string)(const String &);

class Terminal
{
public:
  Terminal();
  void setup();
  void tick();
  void input(tCB_string callback);

private:
  void backspace();
  void clear_buffer();

  String buffer;
  bool input_active;
  tCB_string input_callback;
};

extern Terminal terminal;
