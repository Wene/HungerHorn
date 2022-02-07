#include <Arduino.h>
#include <WiFi.h>

#include "player.h"
#include "config.h"
#include "terminal.h"
#include "network.h"
#include "alarmclock.h"

static Player player;

static void menu_selection(const String &input)
{
  if(input == "w")
  {
    network.scan();
  }
}

void setup()
{
  terminal.setup();
  config.setup();
  player.setup();
  network.setup();
  alarmclock.setup();

  terminal.menu(menu_selection);

  Serial.println(F("Setup done."));
  Serial.println(F("Enter \"w\" to start a WiFi scan"));
}

void loop()
{
  static unsigned long last_time = millis();
  unsigned int now = millis();
  terminal.tick();
  alarmclock.tick();

  if(now > last_time + 5000)
  {
    last_time = now;
    alarmclock.printLocalTime();
  }

//  if(Serial.available())
//  {
//    int input = Serial.read();
//    if(input >= '1' && input <= '9')
//    {
//      int track = input - '0';
//      player.play_track(track);
//    }
//  }
//  player.print_state();
}
