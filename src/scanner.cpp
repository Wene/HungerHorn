#include "scanner.h"

#include <WiFi.h>

#include "terminal.h"
#include "config.h"

static int num_net;
static String net_name;
static String net_password;
static tScanner_callback scanner_callback;

static void net_pw(const String &input)
{
  net_password = input;
  config.store_wifi_settings(net_name, net_password);
  scanner_callback();
}

static void net_select(const String &input)
{
  int net_number = input.toInt();
  if(net_number >= 0 && net_number < num_net)
  {
    net_name = WiFi.SSID(net_number);
  }

  Serial.print(F("Enter the network password for \""));
  Serial.print(net_name);
  Serial.print("\": ");
  terminal.input(net_pw);
}

void scanner_scan(tScanner_callback callback)
{
  scanner_callback = callback;

  Serial.println(F("Scanning for WiFi networks..."));
  num_net = WiFi.scanNetworks();
  Serial.print(F("Scan complete. Found "));
  Serial.print(num_net);
  Serial.println(F(" networks:"));
  for(int i = 0; i < num_net; i++)
  {
    String net_name = String(i) + ": " + WiFi.SSID(i);
    Serial.println(net_name);
  }
  Serial.print(F("Enter the number of your choice: "));
  terminal.input(net_select);
}
