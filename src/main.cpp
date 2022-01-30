#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

#include "player.h"


static Player player;

static Preferences settings;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));

  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();

  settings.begin("settings");

  player.setup();

  Serial.println(F("Setup done."));
}

#define ENTER 13
#define BACKSPACE 127
#define ESC 27

void scan()
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
          Serial.write(ESC);
          Serial.print("[D");
          Serial.write(ESC);
          Serial.print("[K");
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
        else
        {
          Serial.write(in);
        }
      }
    }
  }
  else
  {
    Serial.println(F("not available"));
  }
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
        scan();
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
