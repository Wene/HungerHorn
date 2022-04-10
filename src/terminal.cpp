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
      backspace();
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
      if(!isControl(in_char))
      {
        buffer += String((char)in_char);
        Serial.write(in_char);
      }
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
  if(0 < buffer.length())
  {
    bool multi_byte_char = false;
    do
    {
      uint last_char_index = buffer.length() - 1;
      char deleted_char = buffer.charAt(last_char_index);
      buffer = buffer.substring(0, last_char_index);

      // characters starting with 0b10xxxxxx are part of utf-8 multibyte characters
      multi_byte_char = (deleted_char & 0b10000000) && !(deleted_char & 0b01000000);
    }
    while(multi_byte_char && 0 < buffer.length());

    // move cursor left
    Serial.write(ESC);
    Serial.print("[D");

    // clear the line from cursor to the right
    Serial.write(ESC);
    Serial.print("[K");
  }
}

void Terminal::clear_serial_buffer()
{
  while(Serial.available())
  {
    Serial.read();
  }
}

Terminal terminal;
