#include "terminal.h"

#define ENTER 13
#define BACKSPACE 127
#define ESC 27

void Terminal::setup(std::function<void(void)> callback)
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  menu_callback = callback;
  last_interaction = millis();
  active = false;
}

void Terminal::tick(unsigned long now)
{
  if(active && Serial.available())
  {
    last_interaction = now;
    int in_char = Serial.read();
    if(BACKSPACE == in_char)
    {
      unsigned len = buffer.length();
      if(0 < len)
      {
        buffer = buffer.substring(0, len-1);
        backspace();
      }
    }
    else if(ENTER == in_char)
    {
      Serial.println();
      if(input_callback)
      {
        active = false;
        input_callback(buffer);
      }
      buffer.clear();
      if(!active)
      {
        menu_callback();
      }
    }
    else
    {
      buffer += String((char)in_char);
      Serial.write(in_char);
    }
  }
  else if(now > last_interaction + 30000)
  {
    last_interaction = now;
    menu_callback();
  }
}

void Terminal::input(std::function<void(const String&)> callback)
{
  clear_serial_buffer();
  active = true;
  input_callback = callback;
  last_interaction = millis();
}

void Terminal::backspace()
{
  Serial.write(ESC);
  Serial.print("[D");
  Serial.write(ESC);
  Serial.print("[K");
}

void Terminal::clear_serial_buffer()
{
  while(Serial.available())
  {
    Serial.read();
  }
}

Terminal terminal;
