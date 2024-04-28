#include <math.h>
#include <stdlib.h>

#define SAMPLERATE 48000.0
#define SAMPLE_DURATION 1.0 / SAMPLERATE
#define TWO_PI 2.0 * M_PI

static float outputBuffer[128];

unsigned long long tSamples = 0;
double kickPhase = 0.0, bassPhase = 0.0;

int pattern[16] = {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0};

double saw4f(double phase) {
  return sinf(phase) +
    sinf(phase * 2.0) / 2.0 +
    sinf(phase * 3.0) / 3.0 +
    sinf(phase * 4.0) / 4.0;
}

float* makeSomeTechno() {
  for (unsigned char i = 0; i < 128; i++) {
    double tSeconds = tSamples++ * SAMPLE_DURATION;
    double tBeats = tSeconds * 2.0; // 120 BPM
    int bar = (int)tBeats / 4;
    double tInBeat = fmod(tBeats, 1.0);
    double tSixteenths = tBeats * 4.0;
    int sixteenth = (int)tSixteenths % 16;
    double tInSixteenth = fmod(tSixteenths, 1.0);

    double kickPitchEnv = powf(1.0 - tInBeat, 50.0);
    double kickPitch = 50.0 + 900.0 * kickPitchEnv;
    kickPhase = fmod(kickPhase + SAMPLE_DURATION * TWO_PI * kickPitch, TWO_PI);
    double kickEnv = powf(1.0 - tInBeat, 3.0) * 0.15;
    double kick = sinf(kickPhase) * kickEnv;

    double bassPitch = 50.0;
    bassPhase = fmod(bassPhase + SAMPLE_DURATION * TWO_PI * bassPitch, TWO_PI);
    double bassEnv = (1.0 - pow(2.0, -(1.1 * tInBeat + 0.01 / tInBeat))) * 0.2;
    double bassEnv2 = pow(2.0, -(6.0 * tInSixteenth + 0.01 / tInSixteenth)) * 0.1;
    double bass = tanh(sinf(bassPhase) * 1.5) * (bassEnv + bassEnv2);

    int chordHit = pattern[sixteenth];
    double chordRootPitch = bassPitch * 4;
    double chordRootPhase = chordRootPitch * 2.0 * M_PI * tSeconds * (bar % 4 < 2 ? 1.0 : 2.0/3);
    float chord1 = saw4f(chordRootPhase);
    float chord2 = saw4f(chordRootPhase * 6/5);
    float chord3 = saw4f(chordRootPhase * 3/2);
    double chordEnv = chordHit ? pow(2.0, -(3.0 * tInSixteenth + 0.01 / tInSixteenth)) : 0.0;
    double chordOut = (chord1 + chord2 + chord3) * chordEnv * 0.15;

    outputBuffer[i] = kick + bass + chordOut;
  }

  return outputBuffer;
}
