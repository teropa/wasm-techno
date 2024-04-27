#include <math.h>

#define SR 48000.0
#define SAMPLE_DURATION 1.0 / SR
#define TWO_PI 2.0 * M_PI

static float outputBuffer[128];

unsigned long long tSamples = 0;
float kickPhase = 0.0;

float* makeSomeTechno() {
  for (unsigned char i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DURATION;
    float tBeats = tSeconds * 2.0;
    float tInBeat = fmodf(tBeats, 1.0);

    float kickPitchEnv = powf(1.0 - tInBeat, 20.0);
    float kickPitch = 80.0 + 400.0 * kickPitchEnv;
    kickPhase = fmodf(kickPhase + SAMPLE_DURATION * TWO_PI * kickPitch, TWO_PI);
    float kickEnv = 0.2 * powf(1.0 - tInBeat, 2.0);
    float val = sinf(kickPhase) * kickEnv;

    outputBuffer[i] = val;
  }

  return outputBuffer;
}
