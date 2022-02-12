#include "lightshow.h"
#include <FastLED.h>
#include <time.h>

#define NUM_LEDS 6
#define LED_PIN 23

CRGBArray<NUM_LEDS> leds;

void Lightshow::setup()
{
  FastLED.setBrightness(80);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  FastLED.clear(true);
  FastLED.show();
}

void Lightshow::tick(unsigned long now)
{
  if(now > last_show + 20)
  {
    last_show = now;
    time_t timestamp = time(NULL);
    struct tm *time_struct = localtime(&timestamp);

    if(++duration > 250)
    {
      duration = 0;
      if(++step > 2)
      {
        step = 0;
      }
    }

    int display_value = 0;
    uint8_t display_color = 0;
    switch(step)
    {
      case 0:
        display_value = time_struct->tm_hour;
        display_color = HSVHue::HUE_RED;
        break;
      case 1:
        display_value = time_struct->tm_min;
        display_color = HSVHue::HUE_GREEN;
        break;
      default:
      case 2:
        display_value = time_struct->tm_sec;
        display_color = HSVHue::HUE_BLUE;
        break;
    }

    for(int i = 0; i < NUM_LEDS; i++)
    {
      if(display_value & 1 << i)
      {
        leds[i].setHSV(display_color, 255, 255);
      }
      else
      {
        leds[i].fadeToBlackBy(10);
      }
    }

    FastLED.show();
  }
}
