#include "network.h"

#include <Preferences.h>
#include <WiFi.h>
#include <functional>

#include "terminal.h"

static const char *ssid_prefix = "ssid_";
static const char *psk_prefix = "psk_";

Network::Network()
{
  settings = new Preferences;
}

Network::~Network()
{
  settings->end();
  delete settings;
}

void Network::setup()
{
  settings->begin("wifi");

  int index = 0;
  while(true)
  {
    String ssid_key(ssid_prefix);
    ssid_key += index;
    String psk_key(psk_prefix);
    psk_key += index;

    if(settings->isKey(ssid_key.c_str()) && settings->isKey(psk_key.c_str()))
    {
      WifiConfig config;
      config.ssid = settings->getString(ssid_key.c_str());
      config.psk = settings->getString(psk_key.c_str());
      wifi_list.push_back(config);
    }
    else
    {
      break;
    }

    index++;
  }

  WiFi.mode(WIFI_MODE_STA);

  search();
}

void Network::search()
{
  num_net = WiFi.scanNetworks();
  for(int i = 0; i < num_net; i++)
  {
    String name{WiFi.SSID(i)};
    for(WifiConfig& config : wifi_list)
    {
      if(name == config.ssid)
      {
        active_config = config;
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
  Serial.print(active_config.ssid);
  Serial.print(F("\" using key \""));
  Serial.print(active_config.psk);
  Serial.println("\"...");

  if(WiFi.isConnected())
  {
    WiFi.disconnect();
  }

  WiFi.begin(active_config.ssid.c_str(), active_config.psk.c_str());
  connecting_countdown = 50;
}

void Network::config_start()
{
  Serial.println();
  Serial.println(F("No | Name"));
  int index = 0;
  for(WifiConfig& config : wifi_list)
  {
    String line(index++);
    line += "  : \"";
    line += config.ssid;
    line += "\"";
    Serial.println(line);
  }
  Serial.print(F("Select the network number to edit or enter 'a' to add a new network: "));
  terminal.input(std::bind(&Network::scan, this, std::placeholders::_1));
  setup_active = true;
}

void Network::scan(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  if(input == "a")
  {
    config_slot = wifi_list.size();
  }
  else
  {
    config_slot = input.toInt();
  }

  if(config_slot > wifi_list.size())
  {
    Serial.println(F("Not a valid slot number"));
    setup_active = false;
    return;
  }

  Serial.println(F("Scanning for WiFi networks..."));
  num_net = WiFi.scanNetworks();
  if(num_net < 0)
  {
    num_net = 0;
    Serial.println(F("WiFi scan failed or no networks found."));
    Serial.print(F("Enter 'd' to delete this slot, or just hit enter to abort: "));
  }
  else
  {
    Serial.print(F("Scan complete. Found "));
    Serial.print(num_net);
    Serial.println(F(" networks:"));

    for(int i = 0; i < num_net; i++)
    {
      String name = String(i) + ": " + WiFi.SSID(i);
      Serial.println(name);
    }

    Serial.print(F("Enter the network number of your choice, 'd' to delete this slot, or just hit enter to abort: "));
  }
  terminal.input(std::bind(&Network::select, this, std::placeholders::_1));
}

void Network::select(const String &input)
{
  if(input.isEmpty())
  {
    setup_active = false;
    return;
  }

  if(input == "d")
  {
    if(config_slot < wifi_list.size())
    {
      wifi_list.erase(wifi_list.begin() + config_slot);
    }

    for(unsigned long i = config_slot; i < wifi_list.size(); i++)
    {
      WifiConfig& config = wifi_list[i];
      store_config(config, i);
    }

    String ssid_key{ssid_prefix};
    ssid_key += wifi_list.size();
    String psk_key{psk_prefix};
    psk_key += wifi_list.size();
    settings->remove(ssid_key.c_str());
    settings->remove(psk_key.c_str());
  }
  else
  {
    int net_number = input.toInt();
    if(net_number >= 0 && net_number < num_net)
    {
      active_config.ssid = WiFi.SSID(net_number);

      Serial.print(F("Enter the network password for \""));
      Serial.print(active_config.ssid);
      Serial.print("\": ");
      terminal.input(std::bind(&Network::password, this, std::placeholders::_1));
    }
    else
    {
      Serial.println(F("Invalid selection"));
    }
  }
}

void Network::password(const String &input)
{
  active_config.psk = input;

  if(config_slot >= wifi_list.size())
  {
    wifi_list.push_back(active_config);
  }
  else
  {
    wifi_list[config_slot] = active_config;
  }

  store_config(active_config, config_slot);

  connect();
  setup_active = false;
}

void Network::store_config(WifiConfig& config, unsigned long index)
{
  String ssid_key{ssid_prefix};
  ssid_key += index;
  String psk_key{psk_prefix};
  psk_key += index;
  settings->putString(ssid_key.c_str(), config.ssid);
  settings->putString(psk_key.c_str(), config.psk);
}

Network network;
