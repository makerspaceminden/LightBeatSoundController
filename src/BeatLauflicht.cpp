#include "BeatLauflicht.hpp"

BeatLauflicht::BeatLauflicht(LEDs* leds) {
    _leds = leds;

    setPixels();
}

BeatLauflicht::~BeatLauflicht() { }

void BeatLauflicht::loop(bool isBeat) {
    if (isBeat) {
        _segment++;

        if (_segment == SEGMENT_COUNT) {
            _segment = 0;
        }
    }

    setPixels();
}

void BeatLauflicht::setPixels() {
    auto quarterSize = LED_COUNT / SEGMENT_COUNT;
    uint16_t low = _segment * quarterSize;
    uint16_t high = low + quarterSize;

    for (int i = 0; i < LED_COUNT; i++) {
        if (i >= low && i < high) {
            _leds->setPixel(CRGB(255, 255, 255), i);
        }
    }
}