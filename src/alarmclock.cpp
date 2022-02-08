#include "alarmclock.h"
#include <functional>
#include "config.h"
#include "terminal.h"

void AlarmClock::setup()
{
  ntp_server = config.get_ntp_server();
  utc_offset = config.get_utc_offset_secs();
  dst_offset = config.get_dst_offset_secs();
  configTime(utc_offset, dst_offset, ntp_server.c_str());
}

void AlarmClock::tick()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println(F("Failed to obtain time"));
    return;
  }
  // TODO: react on set time
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

/*
Tuesday, February 01 2022 20:07:32
Day of week: Tuesday
Month: February
Day of Month: 01
Year: 2022
Hour: 20
Hour (12 hour format): 08
Minute: 07
Second: 32
Time variables
20
Tuesday
*/
void AlarmClock::printLocalTime()
{
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

AlarmClock alarmclock;
