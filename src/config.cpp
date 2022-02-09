#include "config.h"
#include <Preferences.h>

static Preferences settings;

static const char *settings_name = "settings";
static const char *psk_name = "psk";
static const char *ssid_name = "ssid";
static const char *ntp_name = "ntp";
static const char *utc_offs_name = "utc_offset";
static const char *dst_name = "dst_offset";
static const char *alarm_hour_name = "alarm_hour";
static const char *alarm_min_name = "alarm_min";
static const char *alarm_sec_name = "alarm_sec";

Config::Config(): ssid("unset"), psk("---"), ntp_server("pool.ntp.org"), utc_offset_secs(0), dst_offset_secs(0)
{
}

void Config::setup()
{
  settings.begin(settings_name);

  if(settings.isKey(ssid_name))
  {
    ssid = settings.getString(ssid_name);
  }

  if(settings.isKey(psk_name))
  {
    psk = settings.getString(psk_name);
  }

  if(settings.isKey(ntp_name))
  {
    ntp_server = settings.getString(ntp_name);
  }

  if(settings.isKey(utc_offs_name))
  {
    utc_offset_secs = settings.getLong(utc_offs_name);
  }

  if(settings.isKey(dst_name))
  {
    dst_offset_secs = settings.getInt(dst_name);
  }

  if(settings.isKey(alarm_hour_name))
  {
    alarm_time.hour = settings.getInt(alarm_hour_name);
  }
  if(settings.isKey(alarm_min_name))
  {
    alarm_time.min = settings.getInt(alarm_min_name);
  }
  if(settings.isKey(alarm_sec_name))
  {
    alarm_time.sec = settings.getInt(alarm_sec_name);
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

const String &Config::get_ntp_server()
{
  return ntp_server;
}

TimeConfig Config::get_alarm_settings()
{
  return alarm_time;
}

long Config::get_utc_offset_secs()
{
  return utc_offset_secs;
}

int Config::get_dst_offset_secs()
{
  return dst_offset_secs;
}

void Config::store_wifi_settings(const String &name, const String &key)
{
  ssid = name;
  psk = key;
  settings.putString(ssid_name, ssid);
  settings.putString(psk_name, psk);
}

void Config::store_clock_settings(const String &ntp_server_address, long utc_offset, int dst_offset)
{
  ntp_server = ntp_server_address;
  utc_offset_secs = utc_offset;
  dst_offset_secs = dst_offset;
  settings.putString(ntp_name, ntp_server);
  settings.putLong(utc_offs_name, utc_offset_secs);
  settings.putInt(dst_name, dst_offset_secs);
}

void Config::store_alarm_settings(const TimeConfig &alarm)
{
  alarm_time = alarm;
  settings.putInt(alarm_hour_name, alarm_time.hour);
  settings.putInt(alarm_min_name, alarm_time.min);
  settings.putInt(alarm_sec_name, alarm_time.sec);
}

Config config;
