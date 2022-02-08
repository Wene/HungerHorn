#include "network.h"

#include <WiFi.h>
#include <functional>

#include "terminal.h"
#include "config.h"


void Network::setup()
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();

  ssid = config.get_wifi_ssid();
  psk = config.get_wifi_psk();

  connect();
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

void Network::scan()
{
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
  Serial.print(F("Enter the number of your choice: "));
  terminal.input(std::bind(&Network::select, this, std::placeholders::_1));
}

void Network::select(const String &input)
{
  int net_number = input.toInt();
  if(net_number >= 0 && net_number < num_net)
  {
    ssid = WiFi.SSID(net_number);
  }

  Serial.print(F("Enter the network password for \""));
  Serial.print(ssid);
  Serial.print("\": ");
  terminal.input(std::bind(&Network::password, this, std::placeholders::_1));
}

void Network::password(const String &input)
{
  psk = input;
  config.store_wifi_settings(ssid, psk);
  connect();
}

Network network;
