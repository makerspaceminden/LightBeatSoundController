#ifndef __HINTERGRUND_H__
#define __HINTERGRUND_H__

#include "LEDs.hpp"
#include "ILichEffekt.hpp"

class Hintergrund : public ILichtEffekt {
private:
    LEDs* _leds;
    CRGB* _colors;
    size_t _colorsLength;

public:
    Hintergrund(LEDs* leds, CRGB* colors, size_t colorsLength);
    ~Hintergrund();

    void loop(bool isBeat);
};

#endif