#include "config.h"
#include <Preferences.h>

static Preferences settings;

static const char *settings_name = "settings";
static const char *ssid_prefix = "ssid_";
static const char *psk_prefix = "psk_";
static const char *ntp_name = "ntp";
static const char *utc_offs_name = "utc_offset";
static const char *dst_name = "dst_offset";
static const char *alarm_time_prefix = "alarm_time_";
static const char *alarm_sound_prefix = "alarm_sound_";

int Alarm::hour()
{
  return secs_in_day / 3600;
}

int Alarm::min()
{
  int min_sec = secs_in_day % 3600;
  return min_sec / 60;
}

int Alarm::sec()
{
  int min_sec = secs_in_day % 3600;
  return min_sec % 60;
}

void Alarm::hour(int new_val)
{
  if(secs_in_day < 0)
  {
    secs_in_day = 0;
  }

  int hour = new_val;
  int rest = secs_in_day % 3600;
  int min = rest / 60;
  int sec = rest % 60;

  secs_in_day = hour * 3600 + min * 60 + sec;
}


void Alarm::min(int new_val)
{
  if(secs_in_day < 0)
  {
    secs_in_day = 0;
  }

  int hour = secs_in_day / 3600;
  int rest = secs_in_day % 3600;
  int min = new_val;
  int sec = rest % 60;

  secs_in_day = hour * 3600 + min * 60 + sec;
}

void Alarm::sec(int new_val)
{
  if(secs_in_day < 0)
  {
    secs_in_day = 0;
  }

  int hour = secs_in_day / 3600;
  int rest = secs_in_day % 3600;
  int min = rest / 60;
  int sec = new_val;

  secs_in_day = hour * 3600 + min * 60 + sec;
}


Config::Config(): ntp_server("pool.ntp.org"), utc_offset_secs(0), dst_offset_secs(0)
{
}

void Config::setup()
{
  settings.begin(settings_name);

  for(int i = 0; i < NUM_WIFI; i++)
  {
    String ssid_key(ssid_prefix);
    ssid_key += i;
    String psk_key(psk_prefix);
    psk_key += i;

    if(settings.isKey(ssid_key.c_str()))
    {
      ssid[i] = settings.getString(ssid_key.c_str());
    }
    if(settings.isKey(psk_key.c_str()))
    {
      psk[i] = settings.getString(psk_key.c_str());
    }
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

  for(int i = 0; i < NUM_ALARM; i++)
  {
    String time_key(alarm_time_prefix);
    time_key += i;
    if(settings.isKey(time_key.c_str()))
    {
      alarm[i].secs_in_day = settings.getInt(time_key.c_str());
    }
    String sound_key(alarm_sound_prefix);
    sound_key += i;
    if(settings.isKey(sound_key.c_str()))
    {
      alarm[i].sound = settings.getUChar(sound_key.c_str());
    }
  }
}

static String invalid_str("invalid");

const String &Config::get_wifi_ssid(int i)
{
  if(0 > i || NUM_WIFI <= i)
  {
    return invalid_str;
  }
  return ssid[i];
}

const String &Config::get_wifi_psk(int i)
{
  if(i < 0 || NUM_WIFI <= i)
  {
    return invalid_str;
  }
  return psk[i];
}

const String &Config::get_ntp_server()
{
  return ntp_server;
}

Alarm Config::get_alarm_settings(int i)
{
  if(0 > i || NUM_ALARM <= i)
  {
    Alarm invalid;
    invalid.secs_in_day = -1;
    return invalid;
  }

  return alarm[i];
}

long Config::get_utc_offset_secs()
{
  return utc_offset_secs;
}

int Config::get_dst_offset_secs()
{
  return dst_offset_secs;
}

void Config::store_wifi_settings(const String &name, const String &key, int i)
{
  if(0 > i || NUM_WIFI <= i)
  {
    return;
  }

  ssid[i] = name;
  psk[i] = key;

  String ssid_key(ssid_prefix);
  ssid_key += i;
  String psk_key(psk_prefix);
  psk_key += i;

  settings.putString(ssid_key.c_str(), ssid[i]);
  settings.putString(psk_key.c_str(), psk[i]);
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

void Config::store_alarm_settings(const Alarm &alarm_ref, int i)
{
  if(0 > i || NUM_ALARM <= i)
  {
    return;
  }

  alarm[i] = alarm_ref;

  String time_key(alarm_time_prefix);
  time_key += i;
  settings.putInt(time_key.c_str(), alarm[i].secs_in_day);

  String sound_key(alarm_sound_prefix);
  sound_key += i;
  settings.putUChar(sound_key.c_str(), alarm[i].sound);
}

Config config;
