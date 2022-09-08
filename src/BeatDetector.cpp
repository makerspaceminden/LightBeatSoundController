#include <WiFi.h>

#include "BeatDetector.hpp"

// Public
const uint8_t BeatDetector::NumBands = 5;

BeatDetector::BeatDetector() { }

BeatDetector::~BeatDetector() { }

bool BeatDetector::isBeat(int bandValues[16]) {
    static unsigned long lastBeat = 0;

    // 0 und 1 basedrume
    // 2 und 3 snare

    for (uint8_t bandIndex = 0; bandIndex < NumBands; bandIndex++) {
        bool isBeat = isBeatForBand(bandValues[bandIndex], bandIndex);

        auto now = millis();

        if (isBeat && now - lastBeat >= 400) {
            // Serial.println("--->");

            lastBeat = now;

            return true;
        }
    }

    return false;
}

// Private
bool BeatDetector::isBeatForBand(int bandValue, int index) {
    static int oldLevel = 0;

    int level = bandValue / 200000;
    int diff = oldLevel - level;
    int diffAdd = diff / 10;
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