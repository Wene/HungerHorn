#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "player.h"
#include "config.h"
#include "terminal.h"
#include "scanner.h"

static Player player;
static Config config;
static Terminal terminal;

static void wifi_setup()
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();

  String wifi_name(config.get_wifi_ssid());
  String wifi_key(config.get_wifi_psk());

  Serial.print(F("Connecting to WLAN \""));
  Serial.print(wifi_name);
  Serial.print(F("\" using key \""));
  Serial.print(wifi_key);
  Serial.print("\"");
  WiFi.begin(wifi_name.c_str(), wifi_key.c_str());

  bool connected = false;
  for(int i = 0; i < 30; i++)
  {
    if(WL_CONNECTED == WiFi.status())
    {
      connected = true;
      break;
    }
    Serial.print(".");
    delay(500);
  }

  if(connected)
  {
    Serial.println(F("success"));
  }
  else
  {
    Serial.println(F("failed"));
    WiFi.disconnect();
  }
}

static void menu_selection(const String &input)
{
  if(input == "w")
  {
    scanner_scan(wifi_setup);
  }
  else
  {
    terminal.input(menu_selection);
  }
}

void setup()
{
  terminal.setup();
  config.setup();
  player.setup();

  wifi_setup();

  terminal.input(menu_selection); // TODO: this works only once like this
  scanner_set_terminal_and_config(&terminal, &config);

  Serial.println(F("Setup done."));
  Serial.println(F("Enter \"w\" to start a WiFi scan"));
}

void loop()
{
  terminal.tick();

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
