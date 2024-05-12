#include <math.h>
#include <stdlib.h>

#define SAMPLERATE 48000.0
#define SAMPLE_DUR 1.0 / SAMPLERATE
#define TWO_PI     2.0 * M_PI

float outputBuffer[128];

unsigned long long tSamples = 0;

float kickPhase   = 0.0,
      bassPhase   = 0.0,
      chordPhase1 = 0.0,
      chordPhase2 = 0.0,
      chordPhase3 = 0.0;

char pattern[16] = {
  0, 1, 0, 0,
  0, 0, 1, 0,
  1, 0, 0, 1,
  0, 0, 1, 0
};

float phasor(float phase, float freq) {
  phase += SAMPLE_DUR * freq; 
  phase -= trunc(phase);
  return phase;
}

float env(float t, float amp, float exp) {
  return powf(1.0 - t, exp) * amp;
}

float saw6f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0) / 2.0 +
    sinf(phase * 3.0) / 3.0 +
    sinf(phase * 4.0) / 4.0 +
    sinf(phase * 5.0) / 5.0 +
    sinf(phase * 6.0) / 6.0;
}


float delayLine[48000] = {0};
int delayIndex = 0;
float processDelay(float input, float feedback, float delayTime) {
  delayLine[delayIndex] = input + delayLine[delayIndex] * feedback;
  delayIndex = (delayIndex + 1) % (int)(delayTime * SAMPLERATE);
  return delayLine[delayIndex];
}

float* step1Sine() {
  for (int i = 0; i < 128; i++) {
    float kickPitch = 200.0;
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sinf(kickPhase * TWO_PI) * 0.15;
    outputBuffer[i] = kick;
  }
  return outputBuffer;
}

float* makeSomeTechno() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR;
    float tBeats = tSeconds * 2.0; // 120 BPM
    int bar = (int)tBeats / 4;
    float tBeatFrac = tBeats - trunc(tBeats);
    float tSixteenths = tBeats * 4.0;
    int sixteenth = (int)tSixteenths % 16;
    float tSixteenthFrac = tSixteenths - trunc(tSixteenths);

    float kickPitchEnv = env(tBeatFrac, 900.0, 50.0);
    float kickPitch = 50.0 + kickPitchEnv;
    kickPhase = phasor(kickPhase, kickPitch);
    float kickEnv = env(tBeatFrac, 0.15, 3.0);
    float kick = sinf(kickPhase * TWO_PI) * kickEnv;

    float bassPitch = 50.0;
    bassPhase = phasor(bassPhase, bassPitch);
    float bassEnv = 0.2 - env(tBeatFrac, 0.2, 0.5);
    float bass = tanh(sinf(bassPhase * TWO_PI) * 1.5) * bassEnv;

    char chordHit = pattern[sixteenth];
    float chordRootPitch = bassPitch * 4;
    if (bar % 4 > 1) chordRootPitch /= 3.0 / 2.0;
    chordPhase1 = phasor(chordPhase1, chordRootPitch);
    chordPhase2 = phasor(chordPhase2, chordRootPitch * 6.0 / 5.0);
    chordPhase3 = phasor(chordPhase3, chordRootPitch * 3.0 / 2.0);
    float chord1 = saw6f(chordPhase1 * TWO_PI);
    float chord2 = saw6f(chordPhase2 * TWO_PI);
    float chord3 = saw6f(chordPhase3 * TWO_PI);
    float chordEnv = env(tSixteenthFrac, 0.1, 2.0) * chordHit;
    float chordOut = (chord1 + chord2 + chord3) * chordEnv;
    chordOut = chordOut + processDelay(chordOut, 0.4, 0.375) * 0.4;

    outputBuffer[i] = tanh(kick + bass + chordOut);
  }

  return outputBuffer;
}
