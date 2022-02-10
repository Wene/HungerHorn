#include "alarmclock.h"
#include <functional>
#include <time.h>
#include "config.h"
#include "terminal.h"
#include "player.h"

void AlarmClock::setup()
{
  ntp_server = config.get_ntp_server();
  utc_offset = config.get_utc_offset_secs();
  dst_offset = config.get_dst_offset_secs();
  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::tick()
{
  unsigned long now = millis();
  if(now >= last_update + 1000)
  {
    TimeConfig alarm = config.get_alarm_settings();
    time_t clock = time(NULL);
    struct tm *timeinfo = localtime(&clock);
    if(alarm.hour == timeinfo->tm_hour && alarm.min == timeinfo->tm_min && alarm.sec == timeinfo->tm_sec)
    {
      player.play_track(1);
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

  Serial.print(F("The current UTC offset in seconds is "));
  Serial.println(utc_offset);
  Serial.print(F("Enter the new value or just hit enter to keep the value: "));
  terminal.input(std::bind(&AlarmClock::config_utc, this, std::placeholders::_1));
}

void AlarmClock::config_utc(const String &input)
{
  if(!input.isEmpty())
  {
    utc_offset = input.toInt();
  }

  Serial.print(F("The current daylight savint time offset in seconds is "));
  Serial.println(dst_offset);
  Serial.print(F("Enter the new value or just hit enter to keep the value: "));
  terminal.input(std::bind(&AlarmClock::config_dst, this, std::placeholders::_1));
}

void AlarmClock::config_dst(const String &input)
{
  if(!input.isEmpty())
  {
    dst_offset = input.toInt();
  }

  config.store_clock_settings(ntp_server, utc_offset, dst_offset);
  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::alarm_setup_start()
{
  TimeConfig alarm = config.get_alarm_settings();
  char cur_alarm[10];
  snprintf(cur_alarm, sizeof(cur_alarm), "%02d:%02d:%02d", alarm.hour, alarm.min, alarm.sec);
  Serial.print(F("The alarm is currently set to "));
  Serial.println(cur_alarm);
  Serial.print(F("Enter the new alarm time: "));
  terminal.input(std::bind(&AlarmClock::alarm_setup, this, std::placeholders::_1));
}

void AlarmClock::alarm_setup(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  int pos = input.indexOf(':');
  if(0 >= pos) return;
  String hour_str = input.substring(0, pos);
  String rest = input.substring(pos+1);
  pos = rest.indexOf(':');
  if(0 >= pos) return;
  String min_str = rest.substring(0, pos);
  String sec_str = rest.substring(pos+1);

  TimeConfig alarm;
  alarm.hour = hour_str.toInt();
  alarm.min = min_str.toInt();
  alarm.sec = sec_str.toInt();

  config.store_alarm_settings(alarm);
}

void AlarmClock::print_time()
{
  time_t timestamp = time(NULL);
  struct tm *timeinfo = localtime(&timestamp);

  Serial.println(timeinfo, "%Y-%m-%d %H:%M:%S");
}

AlarmClock alarmclock;
