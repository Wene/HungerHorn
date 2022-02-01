#include <Arduino.h>
#include <WiFi.h>

#include "player.h"
#include "config.h"
#include "terminal.h"
#include "scanner.h"

static Player player;

static void wifi_setup()
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();

  String wifi_name(config.get_wifi_ssid());
  String wifi_key(config.get_wifi_psk());

  Serial.print(F("Connecting to WLAN \""));
  Serial.print(wifi_name);
  Serial.print(F("\" using key \""));
  Serial.print(wifi_key);
  Serial.print("\"");
  WiFi.begin(wifi_name.c_str(), wifi_key.c_str());

  bool connected = false;
  for(int i = 0; i < 30; i++)
  {
    if(WL_CONNECTED == WiFi.status())
    {
      connected = true;
      break;
    }
    Serial.print(".");
    delay(500);
  }

  if(connected)
  {
    Serial.println(F("success"));
    configTime(config.get_utc_offset_secs(), config.get_dst_offset_secs(), config.get_ntp_server().c_str());
  }
  else
  {
    Serial.println(F("failed"));
    WiFi.disconnect();
  }
}

static void menu_selection(const String &input)
{
  if(input == "w")
  {
    scanner_scan(wifi_setup);
  }
  else
  {
    terminal.input(menu_selection);
  }
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
static void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println(F("Failed to obtain time"));
    return;
  }
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

void setup()
{
  terminal.setup();
  config.setup();
  player.setup();

  wifi_setup();

  terminal.input(menu_selection); // TODO: this works only once like this

  Serial.println(F("Setup done."));
  Serial.println(F("Enter \"w\" to start a WiFi scan"));
}

void loop()
{
  static unsigned long last_time = millis();
  unsigned int now = millis();
  terminal.tick();

  if(now > last_time + 5000)
  {
    last_time = now;
    printLocalTime();
  }

//  if(Serial.available())
//  {
//    int input = Serial.read();
//    if(input >= '1' && input <= '9')
//    {
//      int track = input - '0';
//      player.play_track(track);
//    }
//  }
//  player.print_state();
}
