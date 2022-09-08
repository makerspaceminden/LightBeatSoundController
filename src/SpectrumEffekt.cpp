#include "SpectrumEffekt.hpp"

SpectrumEffekt::SpectrumEffekt(LEDs* leds, int* bandValues) {
    _leds = leds;
    _bandValues = bandValues;

    for (int i = 0; i < LED_COUNT; i++) {
        _colors[i] = CRGB(0, 0, 0);
    }
}

SpectrumEffekt::~SpectrumEffekt() { }

void SpectrumEffekt::loop(bool isBeat) {
    static unsigned long lastFrame = 0;

    auto now = millis();

    bool fadeOut = now - lastFrame >= 33;
    if (fadeOut) lastFrame = now;

    // 0 und 1 basedrume
    // 2 und 3 snare

    for (uint8_t bandIndex = 0; bandIndex < NumBands; bandIndex++) {
        bool isBeat = isBeatForBand(_bandValues[bandIndex], bandIndex);

        int ledsPerBand = LED_COUNT / NumBands;
        int start = bandIndex * ledsPerBand;
        int end = start + ledsPerBand;

        if (isBeat) {
            for (int i = start; i < end; i++) {
                _colors[i] = _bandColors[bandIndex];
            }

        } else if (fadeOut) {
            for (int i = start; i < end; i++) {
                _colors[i].r *= 0.8;
                _colors[i].g *= 0.8;
                _colors[i].b *= 0.8;
            }
        }
    }

    for (uint16_t i = 0; i < LED_COUNT; i++) {
        _leds->setPixel(_colors[i], i);
    }
}

// Private
const uint8_t SpectrumEffekt::NumBands = 5;

bool SpectrumEffekt::isBeatForBand(int bandValue, int index) {
    static int oldLevel = 0;

    int level = bandValue / 200000;
    int diff = oldLevel - level;
    int diffAdd = diff / 4;
    if (diffAdd == 0) {
        diffAdd++;
    }

    oldLevel -= diffAdd;

    if (level > 3 && level > oldLevel) {
        // Serial.printf("%lu\tBand: %d, bandValue: %d, level: %d, old level: %d\n", millis(), index, bandValue, level, oldLevel);

        oldLevel = level;

        return true;
    }

    return false;
}