#include "config.h"
#include <Preferences.h>

Preferences settings;

Config::Config(): ssid("none"), psk("empty")
{

}

static const char *psk_name = "psk";
static const char *ssid_name = "ssid";

void Config::setup()
{
  settings.begin("settings");

  if(settings.isKey(ssid_name))
  {
    ssid = settings.getString(ssid_name);
  }

  if(settings.isKey(psk_name))
  {
    psk = settings.getString(psk_name);
  }
}

const String &Config::get_wifi_ssid()
{
  return ssid;
}

const String &Config::get_wifi_psk()
{
  return psk;
}

void Config::store_wifi_settings(const String &name, const String &key)
{
  ssid = name;
  psk = key;
  settings.putString(ssid_name, ssid);
  settings.putString(psk_name, psk);
}
