#pragma once

#include <Arduino.h>
#include <functional>


class Terminal
{
public:
  Terminal();
  void setup();
  void tick();
  void input(std::function<void(const String&)> callback);

private:
  void backspace();
  void clear_buffer();

  String buffer;
  bool input_active;
  std::function<void(const String&)> input_callback;
};

extern Terminal terminal;
