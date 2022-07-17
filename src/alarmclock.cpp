#include "alarmclock.h"

#include <functional>
#include <time.h>
#include <Preferences.h>

#include "terminal.h"
#include "player.h"

static const char *alarm_time_prefix = "time_";
static const char *alarm_sound_prefix = "sound_";
static const char *ntp_name = "ntp";
static const char *tz_name = "tz";

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

AlarmClock::AlarmClock(): ntp_server{"pool.ntp.org"}, settings{new Preferences}
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

  int index = 0;
  while(true)
  {
    String time_key(alarm_time_prefix);
    time_key += index;
    String sound_key(alarm_sound_prefix);
    sound_key += index;

    if(settings->isKey(time_key.c_str()) && settings->isKey(sound_key.c_str()))
    {
      Alarm alarm;
      alarm.secs_in_day = settings->getInt(time_key.c_str());
      alarm.sound = settings->getUChar(sound_key.c_str());
      alarm_list.push_back(alarm);
    }
    else
    {
      break;
    }

    index++;
  }

  if(settings->isKey(ntp_name))
  {
    ntp_server = settings->getString(ntp_name);
  }
  if(settings->isKey(tz_name))
  {
    tz_str = settings->getString(tz_name);
  }
  setup_time();
}

void AlarmClock::tick(unsigned long now)
{
  if(now >= last_update + 1000ul)
  {
    last_update = now;

    time_t clock = time(NULL);
    struct tm *timeinfo = localtime(&clock);
    int32_t now_secs = timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec;

    for(Alarm& alarm : alarm_list)
    {
      if(now_secs == alarm.secs_in_day && alarm.sound)
      {
        player.play_track(alarm.sound);

        for(auto& event : event_callbacks)
        {
          event();
        }

        last_update += 10000ul;
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

  Serial.print(F("The current timezone is \""));
  Serial.print(tz_str);
  Serial.println("\"");
  Serial.println(F("Find your tomezone info here: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv"));
  Serial.print(F("Enter a new timezome (like \"CET-1CEST,M3.5.0,M10.5.0/3\") or just hit enter to keep the value: "));
  terminal.input(std::bind(&AlarmClock::config_tz, this, std::placeholders::_1));
}

void AlarmClock::config_tz(const String &input)
{
  if(!input.isEmpty())
  {
    tz_str = input;
  }

  settings->putString(ntp_name, ntp_server);
  settings->putString(tz_name, tz_str);

  setup_time();
}

void AlarmClock::alarm_setup_start()
{
  Serial.println();
  Serial.println(F("No | Sound | Time"));

  int index = 0;
  for(Alarm& alarm : alarm_list)
  {
    String line{index++};
    line += "  : ";

    char c_str[10];
    snprintf(c_str, sizeof(c_str), "%-6d: ", alarm.sound);
    line += c_str;

    snprintf(c_str, sizeof(c_str), "%02d:%02d:%02d", alarm.hour(), alarm.min(), alarm.sec());
    line += c_str;

    Serial.println(line);
  }

  Serial.print(F("Enter a number to edit, 'a' to add a new entry, or hit enter to abort: "));
  terminal.input(std::bind(&AlarmClock::alarm_setup_no, this, std::placeholders::_1));
}

void AlarmClock::alarm_setup_no(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  if(input == "a")
  {
    setup_index = alarm_list.size();
    alarm_list.emplace_back(Alarm{});
  }
  else
  {
    setup_index = input.toInt();
  }

  if(setup_index >= 0 && setup_index < alarm_list.size())
  {
    Serial.print(F("Enter the sound number or 'd' to delete this alarm: "));
    terminal.input(std::bind(&AlarmClock::alarm_setup_snd, this, std::placeholders::_1));
    setup_alarm = alarm_list[setup_index];
  }
  else
  {
    Serial.println(F("selection out of range"));
  }
}

void AlarmClock::alarm_setup_snd(const String &input)
{
  if(input.isEmpty())
  {
    return;
  }

  if(input == "d")
  {
    alarm_list.erase(alarm_list.begin() + setup_index);
    for(unsigned long i = setup_index; i < alarm_list.size(); i++)
    {
      Alarm& alarm = alarm_list[i];
      store_alarm(alarm, i);
    }
    String time_key(alarm_time_prefix);
    time_key += alarm_list.size();
    settings->remove(time_key.c_str());
    String sound_key(alarm_sound_prefix);
    sound_key += alarm_list.size();
    settings->remove(sound_key.c_str());
  }
  else
  {
    setup_alarm.sound = input.toInt();
    Serial.print(F("Enter the alarm time: "));
    terminal.input(std::bind(&AlarmClock::alarm_setup_time, this, std::placeholders::_1));
  }
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

  alarm_list[setup_index] = setup_alarm;

  store_alarm(setup_alarm, setup_index);
}

void AlarmClock::print_time()
{
  time_t timestamp = time(NULL);
  struct tm *timeinfo = localtime(&timestamp);

  Serial.println(timeinfo, "%Y-%m-%d %H:%M:%S");
}

void AlarmClock::add_event_callback(std::function<void(void)> event)
{
  event_callbacks.push_back(event);
}

void AlarmClock::store_alarm(const Alarm& alarm, unsigned long index)
{
  String time_key(alarm_time_prefix);
  time_key += index;
  settings->putInt(time_key.c_str(), alarm.secs_in_day);
  String sound_key(alarm_sound_prefix);
  sound_key += index;
  settings->putUChar(sound_key.c_str(), alarm.sound);
}

void AlarmClock::setup_time()
{
  configTime(0, 0, ntp_server.c_str());
  setenv("TZ", tz_str.c_str(), 1);
  tzset();
}

AlarmClock alarmclock;
