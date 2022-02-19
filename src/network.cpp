#include "network.h"

#include <Preferences.h>
#include <WiFi.h>
#include <functional>

#include "terminal.h"

static const char *ssid_prefix = "ssid_";
static const char *psk_prefix = "psk_";

Network::Network()
{
  settings = new Preferences();
}

Network::~Network()
{
  settings->end();
  delete settings;
}

void Network::setup()
{
  settings->begin("wifi");
  for(int i = 0; i < NUM_WIFI; i++)
  {
    String ssid_key(ssid_prefix);
    ssid_key += i;
    String psk_key(psk_prefix);
    psk_key += i;

    if(settings->isKey(ssid_key.c_str()))
    {
      ssid_list[i] = settings->getString(ssid_key.c_str());
    }
    if(settings->isKey(psk_key.c_str()))
    {
      psk_list[i] = settings->getString(psk_key.c_str());
    }
  }

  WiFi.mode(WIFI_MODE_STA);

  search();
}

void Network::search()
{
  num_net = WiFi.scanNetworks();
  for(int i = 0; i < num_net; i++)
  {
    String name(WiFi.SSID(i));
    for(int j = 0; j < NUM_WIFI; j++)
    {
      if(name == ssid_list[j])
      {
        active_ssid = name;
        active_psk = psk_list[j];
        connect();
        return;
      }
    }
  }

  Serial.println(F("No known WLAN found"));
}

void Network::tick(unsigned long now)
{
  if(connecting_countdown)
  {
    if(now > last_tick + 500)
    {
      last_tick = now;
      connecting_countdown--;

      if(WL_CONNECTED == WiFi.status())
      {
        Serial.println(F("WLAN connected"));
        connecting_countdown = 0;
        // TODO: callback for network depending things
      }
      else if(0 == connecting_countdown)
      {
        Serial.println(F("WLAN connection failed"));
        WiFi.disconnect();
      }
    }
  }
  else if(!setup_active)
  {
    if(now > last_tick + 30000)
    {
      last_tick = now;
      if(WL_CONNECTED != WiFi.status())
      {
        search();
      }
    }
  }
}

void Network::connect()
{
  Serial.print(F("Connecting to WLAN \""));
  Serial.print(active_ssid);
  Serial.print(F("\" using key \""));
  Serial.print(active_psk);
  Serial.println("\"...");

  if(WiFi.isConnected())
  {
    WiFi.disconnect();
  }

  WiFi.begin(active_ssid.c_str(), active_psk.c_str());
  connecting_countdown = 50;
}

void Network::config_start()
{
  Serial.println();
  Serial.println(F("No | Name"));
  for(int i = 0; i < NUM_WIFI; i++)
  {
    String line(i);
    line += "  : \"";
    line += ssid_list[i];
    line += "\"";
    Serial.println(line);
  }
  Serial.print(F("Select the slot to edit: "));
  terminal.input(std::bind(&Network::scan, this, std::placeholders::_1));
  setup_active = true;
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
    setup_active = false;
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
    setup_active = false;
    return;
  }

  int net_number = input.toInt();
  if(net_number >= 0 && net_number < num_net)
  {
    active_ssid = WiFi.SSID(net_number);

    Serial.print(F("Enter the network password for \""));
    Serial.print(active_ssid);
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
  active_psk = input;

  String ssid_key(ssid_prefix);
  ssid_key += slot;
  String psk_key(psk_prefix);
  psk_key += slot;
  settings->putString(ssid_key.c_str(), active_ssid);
  settings->putString(psk_key.c_str(), active_psk);

  connect();
  setup_active = false;
}

Network network;
