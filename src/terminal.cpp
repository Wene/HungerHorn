#include "terminal.h"

#define ENTER 13
#define BACKSPACE 127
#define ESC 27

Terminal::Terminal(): input_active(false)
{

}

void Terminal::setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  clear_buffer();
}

void Terminal::tick()
{
  if(!input_active)
  {
    return;
  }

  if(Serial.available())
  {
    int in_char = Serial.read();
    if(BACKSPACE == in_char)
    {
      int len = buffer.length();
      if(0 < len)
      {
        buffer = buffer.substring(0, len-1);
        backspace();
      }
    }
    if(ENTER == in_char)
    {
      Serial.println();
      input_active = false;
      input_callback(buffer);
      buffer.clear();
      if(!input_active)  //can be set from within callback
      {
        input(menu_callback);
      }
    }
    else
    {
      buffer += String((char)in_char);
      Serial.write(in_char);
    }
  }
}

void Terminal::menu(std::function<void (const String &)> callback)
{
  menu_callback = callback;
  input(menu_callback);
}

void Terminal::input(std::function<void(const String&)> callback)
{
  input_active = true;
  clear_buffer();
  input_callback = callback;
}

void Terminal::backspace()
{
  Serial.write(ESC);
  Serial.print("[D");
  Serial.write(ESC);
  Serial.print("[K");
}

void Terminal::clear_buffer()
{
  while(Serial.available())
  {
    Serial.read();
  }
}

Terminal terminal;