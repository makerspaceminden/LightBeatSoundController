#include <Arduino.h>
#include <AudioTools.h>
#include <AudioKitHAL.h>
#include <Wire.h>
#include <RingBuf.h>
#include <arduinoFFT.h>

#include "BeatDetector.hpp"
#include "LEDs.hpp"
#include "BeatLauflicht.hpp"
#include "Hintergrund.hpp"
#include "SpectrumEffekt.hpp"

#define INPUT_BUFFER_SIZE   512
#define AUDIO_BUFFER_SIZE   INPUT_BUFFER_SIZE / 4
#define RING_BUFFER_SIZE    AUDIO_BUFFER_SIZE * 16

#define SAMPLING_FREQ       44100

#define FFT_SAMPLES         INPUT_BUFFER_SIZE * 2
#define NOISE               200

#define BEAT_THRESHOLD       10

#define EFFECT_COUNT          3

// Audio
AudioKit kit;
uint8_t inputbuffer[INPUT_BUFFER_SIZE];
RingBuf<int16_t, RING_BUFFER_SIZE> ringBuffer;

int bandValues[NUM_BANDS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
double vReal[FFT_SAMPLES];
double vImag[FFT_SAMPLES];
arduinoFFT fft(vReal, vImag, FFT_SAMPLES, SAMPLING_FREQ);
unsigned long elapsedTime;
int amplitude = 250000;
unsigned int sampling_period_us;

BeatDetector beatDetector;

// LED
int effectCount = EFFECT_COUNT;
LEDs leds;
BeatLauflicht beatLauflicht(&leds);
// CRGB backgroundColors[4] = {
//   CRGB(0, 255, 0),
//   CRGB(0, 0, 255),
//   CRGB(255, 255, 255),
//   CRGB(0, 0, 255)
// };
CRGB backgroundColors[1] = {
  CRGB(0, 0, 0)
};
Hintergrund hintergrund(&leds, backgroundColors, 1);
SpectrumEffekt spectrum(&leds, bandValues);
ILichtEffekt* lichtEffekte[EFFECT_COUNT] = {
  &hintergrund,
  &spectrum,
  &beatLauflicht
};


void setup(void) {  
  Serial.begin(115200);

  pinMode(PIN_KEY1, INPUT);
  pinMode(PIN_KEY6, INPUT);

#if AUDIOKIT_BOARD == 6
  pinMode(BLUE_LED_GPIO, OUTPUT);
#endif

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));

  auto cfg = kit.defaultConfig(AudioInput);
  cfg.sample_rate = AUDIO_HAL_44K_SAMPLES;
  cfg.adc_input = AUDIO_HAL_ADC_INPUT_LINE2;

  kit.begin(cfg);
}


static int oldlevel;
static unsigned long oldtime = millis();




bool detectBeat(size_t newlyAddedSampleCount) {
  digitalWrite(LED_BUILTIN, LOW);

  size_t ringBufferSize = ringBuffer.size();

  // Serial.printf("Buffer: %d, Samples: %d\n", ringBufferSize, FFT_SAMPLES);

  if (ringBufferSize < FFT_SAMPLES)
    return false;

  // Reset band values
  memset(bandValues, 0, NUM_BANDS * sizeof(int));

  // Reset vImag
  memset(vImag, 0, FFT_SAMPLES * sizeof(double));

  // FFT
  elapsedTime = micros();
  for (int i = 0; i < FFT_SAMPLES; i++) {
    int16_t value;
    ringBuffer.pop(value);

    vReal[i] = (double)value;
    vImag[i] = 0;
  }

  fft.DCRemoval();
  fft.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  fft.Compute(FFT_FORWARD);
  fft.ComplexToMagnitude();

  double bins[3] = { 0, 0, 0 };

  for (int i = 2; i < (FFT_SAMPLES / 2); i++) {
    if (vReal[i] <= NOISE) continue;

    if (i<=2 )           bandValues[0]  += (int)vReal[i];
    if (i>2   && i<=3  ) bandValues[1]  += (int)vReal[i];
    if (i>3   && i<=5  ) bandValues[2]  += (int)vReal[i];
    if (i>5   && i<=7  ) bandValues[3]  += (int)vReal[i];
    if (i>7   && i<=9  ) bandValues[4]  += (int)vReal[i];
    if (i>9   && i<=13 ) bandValues[5]  += (int)vReal[i];
    if (i>13  && i<=18 ) bandValues[6]  += (int)vReal[i];
    if (i>18  && i<=25 ) bandValues[7]  += (int)vReal[i];
    if (i>25  && i<=36 ) bandValues[8]  += (int)vReal[i];
    if (i>36  && i<=50 ) bandValues[9]  += (int)vReal[i];
    if (i>50  && i<=69 ) bandValues[10] += (int)vReal[i];
    if (i>69  && i<=97 ) bandValues[11] += (int)vReal[i];
    if (i>97  && i<=135) bandValues[12] += (int)vReal[i];
    if (i>135 && i<=189) bandValues[13] += (int)vReal[i];
    if (i>189 && i<=264) bandValues[14] += (int)vReal[i];
    if (i>264          ) bandValues[15] += (int)vReal[i];
  }

  // for (uint8_t band = 0; band < BeatDetector::NumBands; band++) {
  //   Serial.printf("Band%d:%d\t", band, bandValues[band] / 200000);
  // }
  // Serial.println();#

  bool isBeat = beatDetector.isBeat(bandValues);

#if AUDIOKIT_BOARD == 6
  if (isBeat) {
    digitalWrite(BLUE_LED_GPIO, LOW);

  } else {
    digitalWrite(BLUE_LED_GPIO, HIGH);
  }
#endif

  return isBeat;
}

void loop() {
  static int mode = 1;
  static int oldMode = 1;

  static unsigned long lastButtonPoll = 0;

  static uint8_t minusButton = 0;
  static uint8_t plusButton = 0;

  if (millis() - lastButtonPoll > 50) {
    minusButton <<= 1;
    if (digitalRead(PIN_KEY1) == LOW) {
      minusButton++;
    }
    switch (minusButton & 0xF) {
      case 0b0010:
      case 0b0110:
      case 0b1110:
        minusButton = 0;
        break;

      case 0b0111:
        mode = max(0, mode - 1);
        minusButton = 0xF;
        break;
      
      default:
        break;
    }

    plusButton <<= 1;
    if (digitalRead(PIN_KEY6) == LOW) {
      plusButton++;
    }
    switch (plusButton & 0xF) {
      case 0b0010:
      case 0b0110:
      case 0b1110:
        plusButton = 0;
        break;

      case 0b0111:
        mode = min(mode + 1, 2);
        plusButton = 0xF;
        break;
      
      default:
        break;
    }

    // Serial.printf("%d\n%d\n\n", minusButton, plusButton);

    if (mode != oldMode) {
      Serial.printf("Mode = %d\n", mode);

      switch (mode) {
        case 0:
          effectCount = 2;
          lichtEffekte[0] = &hintergrund;
          lichtEffekte[1] = &spectrum;
          break;

        case 1:
          effectCount = 3;
          lichtEffekte[0] = &hintergrund;
          lichtEffekte[1] = &spectrum;
          lichtEffekte[2] = &beatLauflicht;
          break;

        case 2:
          effectCount = 2;
          lichtEffekte[0] = &hintergrund;
          lichtEffekte[1] = &beatLauflicht;
          break;

        default:
          break;
      }
    }

    oldMode = mode;
  }


  size_t size = kit.read(inputbuffer, INPUT_BUFFER_SIZE);
  
  // Serial.printf("Size: %d\n\t", size);

  // Audio lesen und in Ringpuffer schreiben
  size_t audioBufferSize = size / 4;
  static int16_t* audioBuffer = (int16_t*)inputbuffer;

  for (size_t i = 0; i < audioBufferSize; i += 2) {
    int16_t left = audioBuffer[i];
    int16_t right = audioBuffer[i + 1];


#if DEBUG
    Serial.printf("%d\t%d\n", left, right);
#endif

    ringBuffer.push(left);
  }

  static unsigned long timestamp = 0;

  bool isBeat = detectBeat(audioBufferSize);

#ifdef PRINT_TIME
  if (isBeat) {
    auto currentTime = millis();

    if (timestamp > 0)
      Serial.printf("Zeit: %ld ms\n", currentTime - timestamp);

    timestamp = currentTime;
  }
#endif

  // Licht
  for (int i = 0; i < effectCount; i++) {
    lichtEffekte[i]->loop(isBeat);
  }

  leds.show();
}
