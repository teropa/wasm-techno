#include <math.h>

static float outputBuffer[128];
unsigned long long tSamples = 0;

float* makeSomeTechno() {
  for (unsigned char i = 0; i < 128; i++) {
    float tSeconds = tSamples++ / 48000.0;
    float tBeats = tSeconds * 2.0;
    float tInBeat = fmodf(tBeats, 1.0);

    float env = 0.2 * (1.0 - tInBeat);
    float val = sinf(tSeconds * 2.0 * M_PI * 220.0) * env;

    outputBuffer[i] = val;
  }

  return outputBuffer;
}
