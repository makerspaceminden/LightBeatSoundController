#include "Hintergrund.hpp"
#include "config.h"

Hintergrund::Hintergrund(LEDs* leds, CRGB* colors, size_t colorsLength) {
    _leds = leds;
    _colors = colors;
    _colorsLength = colorsLength;
}

Hintergrund::~Hintergrund() { }

void Hintergrund::loop(bool isBeat) {
    static size_t step = 0;

    for (int i = 0; i < LED_COUNT; i++) {
        auto color = _colors[(step + i) % _colorsLength];

        _leds->setPixel(color, i);
    }

    if (isBeat) step++;
}