#include <Arduino.h>
#include "player.h"

static Player player;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));

  player.setup();

  Serial.println(F("Setup done."));
}

void loop()
{
  if(Serial.available())
  {
    int input = Serial.read();
    if(input >= '1' && input <= '9')
    {
      int track = input - '0';
      player.play_track(track);
    }
  }
  player.print_state();
}
