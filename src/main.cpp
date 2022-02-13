#include <Arduino.h>
#include <WiFi.h>

#include "player.h"
#include "config.h"
#include "terminal.h"
#include "network.h"
#include "alarmclock.h"
#include "lightshow.h"

static void menu_display();

static void menu_selection(const String &input)
{
  if(input == "w")
  {
    network.config_start();
  }
  else if(input == "c")
  {
    alarmclock.config_start();
  }
  else if(input == "a")
  {
    alarmclock.alarm_setup_start();
  }
  else if(input == "t")
  {
    alarmclock.print_time();
    menu_display();
  }
  else
  {
    Serial.print(F("Selection \""));
    Serial.print(input);
    Serial.println(F("\" not found."));
    menu_display();
  }
}

static void menu_display()
{
  Serial.println();
  Serial.println(F("Menu - enter the letter in front of the entry"));
  Serial.println(F("w - WiFi scan and configuration"));
  Serial.println(F("c - clock setup (NTP server, time zone)"));
  Serial.println(F("a - alarm time settings"));
  Serial.println(F("t - display the current time"));
  terminal.input(menu_selection);
}

void setup()
{
  terminal.setup(menu_display);
  config.setup();
  lightshow.setup();
  network.setup();
  alarmclock.setup();
  player.setup();

  Serial.println(F("Setup done."));
  menu_display();
}

void loop()
{
  unsigned long now = millis();
  terminal.tick(now);
  alarmclock.tick(now);
  lightshow.tick(now);
}
