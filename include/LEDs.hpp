#ifndef __LEDS___H_
#define __LEDS___H_

#include <SPI.h>
#include <FastLED.h>
#include "config.h"

class LEDs {
private:
    CRGBArray<LED_COUNT> leds;

public:
    LEDs(/* args */) {
        FastLED.addLeds<WS2812B, LED_PIN>(leds, LED_COUNT);
    }

    ~LEDs() { };

    void setPixel(CRGB color, uint16_t ledNumber) {
        if (ledNumber < LED_COUNT) {
            leds[ledNumber] = color;
        }
    }

    void show() {
        FastLED.show();
    }
};

#endif