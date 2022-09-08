#ifndef __BEAT_LAUFLICHT___H_
#define __BEAT_LAUFLICHT___H_

#include "LEDs.hpp"
#include "ILichEffekt.hpp"

class BeatLauflicht : public ILichtEffekt {
private:
    LEDs* _leds;
    int _segment = 0;
    const int SEGMENT_COUNT = 4;

    void setPixels();

public:
    BeatLauflicht(LEDs* leds);
    ~BeatLauflicht();

    void loop(bool isBeat);
};

#endif