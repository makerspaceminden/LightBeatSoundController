#ifndef __BEAT_DETECTOR__H_
#define __BEAT_DETECTOR__H_

#include <stdint.h>

class BeatDetector {
private:
    bool isBeatForBand(int bandValue, int index);

public:
    static const uint8_t NumBands;

    BeatDetector();
    ~BeatDetector();

    bool isBeat(int bandValues[16]);
};

#endif