#ifndef __SPECTRUM_EFFEKT__H_
#define __SPECTRUM_EFFEKT__H_

#include <stddef.h>

#include "LEDs.hpp"
#include "ILichEffekt.hpp"

class SpectrumEffekt : public ILichtEffekt {
private:
    LEDs* _leds;
    int* _bandValues;
    CRGB _colors[LED_COUNT];
    CRGB _bandColors[5] = {
        CRGB(0, 255, 0),
        CRGB(128, 255, 0),
        CRGB(255, 255, 0),
        CRGB(255, 0, 0),
        CRGB(0, 255, 255)
    };

    static const uint8_t NumBands;

    bool isBeatForBand(int bandValue, int index);

public:
    SpectrumEffekt(LEDs* leds, int* bandValues);
    ~SpectrumEffekt();

    void loop(bool isBeat);
};

#endif