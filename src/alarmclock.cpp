#include "alarmclock.h"
#include <functional>
#include <time.h>
#include "terminal.h"
#include "player.h"
#include "lightshow.h"

void AlarmClock::setup()
{
  ntp_server = config.get_ntp_server();
  utc_offset = config.get_utc_offset_secs();
  dst_offset = config.get_dst_offset_secs();
  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::tick(unsigned long now)
{
  if(now >= last_update + 1000)
  {
    last_update = now;

    for(int i = 0; i < NUM_ALARM; i++)
    {
      Alarm alarm = config.get_alarm_settings(i);
      if(alarm.secs_in_day < 0) continue;

      time_t clock = time(NULL);
      struct tm *timeinfo = localtime(&clock);
      int32_t now_secs = timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec;

      if(now_secs == alarm.secs_in_day && alarm.sound)
      {
        player.play_track(alarm.sound);
        lightshow.event();
        last_update += 10000;
      }
    }
  }
}

void AlarmClock::config_start()
{
  Serial.print(F("The current NTP server address is \""));
  Serial.print(config.get_ntp_server());
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

  config.store_clock_settings(ntp_server, utc_offset, dst_offset);
  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::alarm_setup_start()
{
  Serial.println();
  Serial.println(F("No | Active | Sound | Time"));

  for(int i = 0; i < NUM_ALARM; i++)
  {
    Alarm alarm = config.get_alarm_settings(i);
    String line(i);
    line += "  : ";

    bool active = alarm.secs_in_day >= 0;
    line += active ? "yes    : " : "no     : ";
    if(!active)
    {
      alarm.secs_in_day = 0;
    }

    char c_str[10];
    snprintf(c_str, sizeof(c_str), "%-6d: ", alarm.sound);
    line += c_str;

    snprintf(c_str, sizeof(c_str), "%02d:%02d:%02d", alarm.hour(), alarm.min(), alarm.sec());
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
    setup_alarm = config.get_alarm_settings(setup_index);
    Serial.print(F("Enter 1 to enable, 0 to disable: "));
    terminal.input(std::bind(&AlarmClock::alarm_setup_act, this, std::placeholders::_1));
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
    config.store_alarm_settings(setup_alarm, setup_index);
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

  config.store_alarm_settings(setup_alarm, setup_index);
}

void AlarmClock::print_time()
{
  time_t timestamp = time(NULL);
  struct tm *timeinfo = localtime(&timestamp);

  Serial.println(timeinfo, "%Y-%m-%d %H:%M:%S");
}

AlarmClock alarmclock;
