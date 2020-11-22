#include <Adafruit_NeoPixel.h>

#include "display.h"

#define GPIO_LEDS (4)
Adafruit_NeoPixel pixels(16, GPIO_LEDS, NEO_GRB + NEO_KHZ800);

static void blink(uint8_t pixel, uint8_t blinks)
{
   for (uint8_t i = 0; i < blinks; i++)
   {
      pixels.clear();
      pixels.show();
      delay(250);

      pixels.setPixelColor(pixel, pixels.Color(0, 150, 0));
      pixels.show();
      delay(250);
   }
}

void Display_Init(uint8_t origin)
{
   pixels.begin();
   pixels.setBrightness(10);
   pixels.setPixelColor(origin, pixels.Color(150, 0, 0));
   pixels.show();
}

void Display_EnterCalibration(uint8_t origin)
{
   // clear the display
   pixels.clear();
   pixels.show();

   // Incrementally light up all LEDs
   uint8_t led = origin;
   for (int i = 0; i < 16; i++)
   {
      pixels.setPixelColor(led, pixels.Color(0, 150, 0));
      pixels.show();
      delay(50);

      led = (led + 1) % 16;
   }

   Display_SetDirection(origin);
}

void Display_ExitCalibration(uint8_t origin)
{
   blink(origin, 2);
   
   pixels.clear();
   pixels.show();
   delay(250);
   
   pixels.setPixelColor(origin, pixels.Color(150, 0, 0));
   pixels.show();
}

void Display_SetDirection(uint8_t pixel)
{
   // Light up the front LED
   pixels.clear();
   pixels.setPixelColor(pixel, pixels.Color(0, 150, 0));
   pixels.show();
}