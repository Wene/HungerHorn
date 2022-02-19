#include "alarmclock.h"

#include <functional>
#include <time.h>
#include <Preferences.h>

#include "terminal.h"
#include "player.h"
#include "lightshow.h"

static const char *alarm_time_prefix = "time_";
static const char *alarm_sound_prefix = "sound_";
static const char *ntp_name = "ntp";
static const char *utc_offs_name = "utc_offset";
static const char *dst_name = "dst_offset";

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

AlarmClock::AlarmClock(): ntp_server("pool.ntp.org"), settings(new Preferences())
{

}

AlarmClock::~AlarmClock()
{
  settings->end();
  delete settings;
}

void AlarmClock::setup()
{
  settings->begin("alarm");

  for(int i = 0; i < NUM_ALARM; i++)
  {
    String time_key(alarm_time_prefix);
    time_key += i;
    if(settings->isKey(time_key.c_str()))
    {
      alarm[i].secs_in_day = settings->getInt(time_key.c_str());
    }
    String sound_key(alarm_sound_prefix);
    sound_key += i;
    if(settings->isKey(sound_key.c_str()))
    {
      alarm[i].sound = settings->getUChar(sound_key.c_str());
    }
  }

  if(settings->isKey(ntp_name))
  {
    ntp_server = settings->getString(ntp_name);
  }
  if(settings->isKey(utc_offs_name))
  {
    utc_offset = settings->getLong(utc_offs_name);
  }
  if(settings->isKey(dst_name))
  {
    dst_offset = settings->getInt(dst_name);
  }
  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::tick(unsigned long now)
{
  if(now >= last_update + 1000)
  {
    last_update = now;

    for(int i = 0; i < NUM_ALARM; i++)
    {
      if(alarm[i].secs_in_day < 0) continue;

      time_t clock = time(NULL);
      struct tm *timeinfo = localtime(&clock);
      int32_t now_secs = timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec;

      if(now_secs == alarm[i].secs_in_day && alarm[i].sound)
      {
        player.play_track(alarm[i].sound);
        lightshow.event();
        last_update += 10000;
      }
    }
  }
}

void AlarmClock::config_start()
{
  Serial.print(F("The current NTP server address is \""));
  Serial.print(ntp_server);
  Serial.println("\"");
  Serial.print(F("Enter the new address or just hit enter to keep it: "));
  terminal.input(std::bind(&AlarmClock::config_ntp, this, std::placeholders::_1));
}

void AlarmClock::config_ntp(const String &input)
{
  if(!input.isEmpty())
  {
    ntp_server = input;
  }

  Serial.print(F("The current UTC offset in minutes is "));
  Serial.println(utc_offset / 60);
  Serial.print(F("Enter the new value or just hit enter to keep the value: "));
  terminal.input(std::bind(&AlarmClock::config_utc, this, std::placeholders::_1));
}

void AlarmClock::config_utc(const String &input)
{
  if(!input.isEmpty())
  {
    utc_offset = input.toInt() * 60;
  }

  Serial.print(F("Daylight saving time is "));
  if(dst_offset)
  {
    Serial.println(F("enabled"));
  }
  else
  {
    Serial.println(F("disabled"));
  }
  Serial.print(F("Enter 1 to enable or 0 to disable. Hit enter to keep the setting: "));
  terminal.input(std::bind(&AlarmClock::config_dst, this, std::placeholders::_1));
}

void AlarmClock::config_dst(const String &input)
{
  if(!input.isEmpty())
  {
    int enable_dst = input.toInt();
    if(enable_dst)
    {
      dst_offset = 3600;
    }
    else
    {
      dst_offset = 0;
    }
  }

  settings->putString(ntp_name, ntp_server);
  settings->putLong(utc_offs_name, utc_offset);
  settings->putInt(dst_name, dst_offset);

  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::alarm_setup_start()
{
  Serial.println();
  Serial.println(F("No | Active | Sound | Time"));

  for(int i = 0; i < NUM_ALARM; i++)
  {
    String line(i);
    line += "  : ";

    bool active = alarm[i].secs_in_day >= 0;
    line += active ? "yes    : " : "no     : ";
    if(!active)
    {
      alarm[i].secs_in_day = 0;
    }

    char c_str[10];
    snprintf(c_str, sizeof(c_str), "%-6d: ", alarm[i].sound);
    line += c_str;

    snprintf(c_str, sizeof(c_str), "%02d:%02d:%02d", alarm[i].hour(), alarm[i].min(), alarm[i].sec());
    line += c_str;
    Serial.println(line);
  }

  Serial.print(F("Enter a number to edit, hit enter to abort: "));
  terminal.input(std::bind(&AlarmClock::alarm_setup_no, this, std::placeholders::_1));
}

void AlarmClock::alarm_setup_no(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  setup_index = input.toInt();
  if(setup_index >= 0 && setup_index < NUM_ALARM)
  {
    Serial.print(F("Enter 1 to enable, 0 to disable: "));
    terminal.input(std::bind(&AlarmClock::alarm_setup_act, this, std::placeholders::_1));
    setup_alarm = alarm[setup_index];
  }
  else
  {
    Serial.println(F("selection out of range"));
  }
}

void AlarmClock::alarm_setup_act(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  int enable = input.toInt();
  if(enable)
  {
    Serial.print(F("Enter the sound number: "));
    terminal.input(std::bind(&AlarmClock::alarm_setup_snd, this, std::placeholders::_1));
  }
  else
  {
    setup_alarm.secs_in_day = -1;
    store_setup_alarm();
  }
}

void AlarmClock::alarm_setup_snd(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  setup_alarm.sound = input.toInt();
  Serial.print(F("Enter the alarm time: "));
  terminal.input(std::bind(&AlarmClock::alarm_setup_time, this, std::placeholders::_1));
}

void AlarmClock::alarm_setup_time(const String &input)
{
  if(!input.isEmpty())
  {
    int pos = input.indexOf(':');
    if(0 >= pos)
    {
      Serial.println(F("invalid format - abort"));
      return;
    }
    String hour_str = input.substring(0, pos);
    String rest = input.substring(pos+1);
    pos = rest.indexOf(':');
    String min_str, sec_str;
    if(0 >= pos)
    {
      min_str = rest;
    }
    else
    {
      min_str = rest.substring(0, pos);
      sec_str = rest.substring(pos+1);
    }

    setup_alarm.hour(hour_str.toInt());
    setup_alarm.min(min_str.toInt());
    setup_alarm.sec(sec_str.toInt());
  }

  store_setup_alarm();
}

void AlarmClock::print_time()
{
  time_t timestamp = time(NULL);
  struct tm *timeinfo = localtime(&timestamp);

  Serial.println(timeinfo, "%Y-%m-%d %H:%M:%S");
}

void AlarmClock::store_setup_alarm()
{
  alarm[setup_index] = setup_alarm;

  String time_key(alarm_time_prefix);
  time_key += setup_index;
  settings->putInt(time_key.c_str(), alarm[setup_index].secs_in_day);
  String sound_key(alarm_sound_prefix);
  sound_key += setup_index;
  settings->putUChar(sound_key.c_str(), alarm[setup_index].sound);

}

AlarmClock alarmclock;
