#include "lightshow.h"
#include <FastLED.h>
#include <time.h>

constexpr int num_leds = 6;
constexpr int leds_pin = 23;

CRGBArray<num_leds> leds;

void Lightshow::setup()
{
  FastLED.setBrightness(80);
  FastLED.addLeds<WS2812, leds_pin, GRB>(leds, leds.size());

  FastLED.clear(true);
  for(auto& led : leds)
  {
    led.setRGB(255, 0, 255);
  }
  FastLED.show();
}

void Lightshow::tick(unsigned long now)
{
  if(now > last_show + 20)
  {
    last_show = now;
    time_t timestamp = time(NULL);
    struct tm *time_struct = localtime(&timestamp);

    if(++duration > 500)
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

    if(event_countdown)
    {
      event_countdown--;
      uint8_t color = (event_countdown / 10) % leds.size();
      uint8_t step = 255 / leds.size();
      color *= step;
      for(auto& led: leds)
      {
        led.setHSV(color, 255, 255);
        color += step;
      }
    }
    else
    {
      uint8_t pos = 0;
      for(auto& led : leds)
      {
        if(display_value & 1 << pos++)
        {
          led.setHSV(display_color, 255, 255);
        }
        else
        {
          led.fadeToBlackBy(10);
        }
      }
    }

    FastLED.show();
  }
}

void Lightshow::event()
{
  event_countdown = 200;
}

Lightshow lightshow;
