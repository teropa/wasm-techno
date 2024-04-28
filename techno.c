#include <math.h>

#define SAMPLERATE 48000.0
#define SAMPLE_DURATION 1.0 / SAMPLERATE
#define TWO_PI 2.0 * M_PI

static float outputBuffer[128];

unsigned long long tSamples = 0;
double kickPhase = 0.0, bassPhase = 0.0;

float* makeSomeTechno() {
  for (unsigned char i = 0; i < 128; i++) {
    double tSeconds = tSamples++ * SAMPLE_DURATION;
    double tBeats = tSeconds * 2.0; // 120 BPM
    double tInBeat = fmod(tBeats, 1.0);
    double tSixteenths = tBeats * 4.0;
    double tInSixteenth = fmod(tSixteenths, 1.0);

    double kickPitchEnv = powf(1.0 - tInBeat, 50.0);
    double kickPitch = 50.0 + 900.0 * kickPitchEnv;
    kickPhase = fmodf(kickPhase + SAMPLE_DURATION * TWO_PI * kickPitch, TWO_PI);
    double kickEnv = powf(1.0 - tInBeat, 3.0) * 0.15;
    double kick = sinf(kickPhase) * kickEnv;

    double bassPitch = 50.0;
    bassPhase = fmod(bassPhase + SAMPLE_DURATION * TWO_PI * bassPitch, TWO_PI);
    int downbeat = ((int)tSixteenths) % 4 == 0;
    double bassEnv = downbeat ? 0.0 : pow(2.0, -(4.0 * tInSixteenth + 0.01 / tInSixteenth)) * 0.4;
    double bass = tanh(sinf(bassPhase) * 2.5) * bassEnv;

    outputBuffer[i] = kick + bass;
  }

  return outputBuffer;
}
