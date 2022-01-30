#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "player.h"
#include "config.h"

static Player player;
static Config config;

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

#define ENTER 13
#define BACKSPACE 127
#define ESC 27

static void terminal_backspace()
{
  Serial.write(ESC);
  Serial.print("[D");
  Serial.write(ESC);
  Serial.print("[K");
}

static void wifi_scan()
{
  Serial.println(F("Scanning for WiFi networks..."));
  int num_net = WiFi.scanNetworks();
  Serial.print(F("Scan complete. Found "));
  Serial.print(num_net);
  Serial.println(F(" networks:"));
  for(int i = 0; i < num_net; i++)
  {
    String net_name = String(i) + ": " + WiFi.SSID(i);
    Serial.println(net_name);
  }

  Serial.print(F("Enter the number of your choice: "));
  String input;
  while(true)
  {
    if(Serial.available())
    {
      int in = Serial.read();
      if(isdigit(in))
      {
        input += String((char)in);
        Serial.write(in);
      }
      else if(BACKSPACE == in)
      {
        int len = input.length();
        if(1 <= len)
        {
          input = input.substring(0, len-1);
          terminal_backspace();
        }
      }
      else if(ENTER == in)
      {
        Serial.println();
        break;
      }
    }
  }
  int selection = input.toInt();
  String net_name;
  if(0 <= selection && num_net > selection)
  {
    net_name = WiFi.SSID(selection);
    Serial.print(F("You have selected \""));
    Serial.print(net_name);
    Serial.println("\"");
    Serial.print(F("Enter the WLAN key: "));
    String key;
    while(true)
    {
      if(Serial.available())
      {
        int in = Serial.read();
        if(ENTER == in)
        {
          Serial.println();
          break;
        }
        else if(BACKSPACE == in)
        {
          int len = key.length();
          if(1 <= len)
          {
            key = key.substring(0, len-1);
            terminal_backspace();
          }
        }
        else
        {
          key += String((char)in);
          Serial.write(in);
        }
      }
    }
    config.store_wifi_settings(net_name, key);
    wifi_setup();
  }
  else
  {
    Serial.println(F("not available"));
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));

  config.setup();

  player.setup();

  wifi_setup();

  Serial.println(F("Setup done."));
}

void loop()
{
  Serial.println(F("Press \"s\" to start a network scan"));
  for(int i = 0; i < 100; i++)
  {
    if(Serial.available())
    {
      int character = Serial.read();
      if(character == 's')
      {
        wifi_scan();
        break;
      }
    }
    delay(100);
  }


  delay(10000);

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
