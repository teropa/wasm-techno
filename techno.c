#include <math.h>

static float outputBuffer[256];

float phase = 0.0;

float* makeSomeTechno() {

  float freqHz = 220.0;
  float freqRad = 2.0 * M_PI * freqHz;
  float phaseIncrement = freqRad / 48000.0;

  for (unsigned char i = 0; i < 128; i++) {
    float val = sinf(phase) * 0.2;
    phase += phaseIncrement;
    outputBuffer[i] = val;
    outputBuffer[i + 128] = val;
  }
  return outputBuffer;
}