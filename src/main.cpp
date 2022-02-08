#include <Arduino.h>
#include <WiFi.h>

#include "player.h"
#include "config.h"
#include "terminal.h"
#include "network.h"
#include "alarmclock.h"

static Player player;

static void menu_display();

static void menu_selection(const String &input)
{
  if(input == "w")
  {
    network.scan();
  }
  else if(input == "c")
  {
    alarmclock.config_start();
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
  Serial.println(F("Enter \"w\" to start a WiFi scan or \"c\" to setup the clock"));
  terminal.input(menu_selection);
}

void setup()
{
  terminal.setup(menu_display);
  config.setup();
  player.setup();
  network.setup();
  alarmclock.setup();

  Serial.println(F("Setup done."));
}

void loop()
{
  terminal.tick();
  alarmclock.tick();
}
