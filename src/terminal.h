#pragma once

#include <Arduino.h>
#include <functional>

class Terminal
{
public:
  void setup(std::function<void(void)> callback);
  void tick();
  void input(std::function<void(const String&)> callback);

private:
  void backspace();
  void clear_serial_buffer();

  unsigned long last_interaction;
  String buffer;
  bool active;
  std::function<void(const String&)> input_callback;
  std::function<void(void)> menu_callback;
};

extern Terminal terminal;
