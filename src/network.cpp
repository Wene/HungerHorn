#include "network.h"

#include <WiFi.h>
#include <functional>

#include "terminal.h"
#include "config.h"


void Network::setup()
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();

  num_net = WiFi.scanNetworks();
  for(int i = 0; i < num_net; i++)
  {
    String name(WiFi.SSID(i));
    for(int j = 0; j < NUM_WIFI; j++)
    {
      if(name == config.get_wifi_ssid(j))
      {
        ssid = name;
        psk = config.get_wifi_psk(j);
        connect();
        return;
      }
    }
  }

  Serial.println(F("No known WLAN found"));
}

void Network::connect()
{
  Serial.print(F("Connecting to WLAN \""));
  Serial.print(ssid);
  Serial.print(F("\" using key \""));
  Serial.print(psk);
  Serial.print("\"");

  WiFi.begin(ssid.c_str(), psk.c_str());

  // this part is blocking: // TODO: unblock
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

void Network::config_start()
{
  Serial.println();
  Serial.println(F("No | Name"));
  for(int i = 0; i < NUM_WIFI; i++)
  {
    String line(i);
    line += "  : \"";
    line += config.get_wifi_ssid(i);
    line += "\"";
    Serial.println(line);
  }
  Serial.print(F("Select the slot to edit: "));
  terminal.input(std::bind(&Network::scan, this, std::placeholders::_1));
}

void Network::scan(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  slot = input.toInt();
  if(0 > slot || NUM_WIFI <= slot)
  {
    Serial.println(F("Not a valid slot number"));
    return;
  }

  Serial.println(F("Scanning for WiFi networks..."));
  num_net = WiFi.scanNetworks();
  Serial.print(F("Scan complete. Found "));
  Serial.print(num_net);
  Serial.println(F(" networks:"));
  for(int i = 0; i < num_net; i++)
  {
    String name = String(i) + ": " + WiFi.SSID(i);
    Serial.println(name);
  }
  Serial.print(F("Enter the number of your choice or just hit enter to abort: "));
  terminal.input(std::bind(&Network::select, this, std::placeholders::_1));
}

void Network::select(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  int net_number = input.toInt();
  if(net_number >= 0 && net_number < num_net)
  {
    ssid = WiFi.SSID(net_number);

    Serial.print(F("Enter the network password for \""));
    Serial.print(ssid);
    Serial.print("\": ");
    terminal.input(std::bind(&Network::password, this, std::placeholders::_1));
  }
  else
  {
    Serial.println(F("Invalid selection"));
  }
}

void Network::password(const String &input)
{
  psk = input;
  config.store_wifi_settings(ssid, psk, slot);
  connect();
}

Network network;
