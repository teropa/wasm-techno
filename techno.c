#include <math.h>

static float outputBuffer[128];
unsigned long long samples = 0;

float* makeSomeTechno() {
  for (unsigned char i = 0; i < 128; i++) {
    float seconds = samples++ / 48000.0;

    float val = sinf(seconds * 2.0 * M_PI * 220.0) * 0.2;

    outputBuffer[i] = val;
  }

  return outputBuffer;
}