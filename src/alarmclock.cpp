#include "alarmclock.h"

void AlarmClock::setup()
{

}

void AlarmClock::tick()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println(F("Failed to obtain time"));
    return;
  }
  // TODO: react on set time
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
