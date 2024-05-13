#include <math.h>
#include <stdlib.h>

#define SAMPLERATE 48000.0f
#define SAMPLE_DUR 1.0f / SAMPLERATE
#define TWO_PI     6.283185307179586f

float outputBuffer[128];

unsigned long long tSamples = 0;

float kickPhase   = 0.0f,
      bassPhase   = 0.0f,
      chordPhase1 = 0.0f,
      chordPhase2 = 0.0f,
      chordPhase3 = 0.0f;

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

float linearEnvelope(float t, float amp) {
  return (1.0f - t) * amp;
}

float expEnvelope(float t, float amp, float exp) {
  return powf(1.0f - t, exp) * amp;
}

float saw6f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0f) / 2.0f +
    sinf(phase * 3.0f) / 3.0f +
    sinf(phase * 4.0f) / 4.0f +
    sinf(phase * 5.0f) / 5.0f +
    sinf(phase * 6.0f) / 6.0f;
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
    float kickPitch = 200.0f;
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sinf(kickPhase * TWO_PI) * 0.15f;
    outputBuffer[i] = kick;
  }
  return outputBuffer;
}

float* step2LinearEnv() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)
    float kickPitch = 200.0f; // Hz
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= linearEnvelope(tBeatFrac, 0.15f); // Shape the amplitude
    outputBuffer[i] = kick; // Output
  }
  return outputBuffer;
}

float* step3ExponentialEnv() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)
    float kickPitch = 200.0f; // Hz
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= expEnvelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude
    outputBuffer[i] = kick; // Output
  }
  return outputBuffer;
}

float* step4KickPitchEnv1() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)
    float kickPitchEnv = expEnvelope(tBeatFrac, 200.0f, 3.0);
    float kickPitch = 50.0 + kickPitchEnv; // Hz
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= expEnvelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude
    outputBuffer[i] = kick; // Output
  }
  return outputBuffer;
}

float* step5KickPitchEnv2() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)
    float kickPitchEnv = expEnvelope(tBeatFrac, 900.0f, 50.0);
    float kickPitch = 50.0 + kickPitchEnv; // Hz
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= expEnvelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude
    outputBuffer[i] = kick; // Output
  }
  return outputBuffer;
}

float* step6BassSine() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)

    float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

    float bassPitch = 50.0f; // Hz
    bassPhase = phasor(bassPhase, bassPitch);
    float bass = sinf(bassPhase * TWO_PI); // Sine wave
    bass *= 0.2f; // Loudness

    outputBuffer[i] = kick + bass; // Output
  }
  return outputBuffer;
}

float* step7BassSaturation1() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)

    float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

    float bassPitch = 50.0f; // Hz
    bassPhase = phasor(bassPhase, bassPitch);
    float bass = sinf(bassPhase * TWO_PI); // Sine wave
    bass = tanh(bass); // More saturation
    bass *= 0.2f; // Loudness

    outputBuffer[i] = kick + bass; // Output
  }
  return outputBuffer;
}

float* step8BassSaturation2() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time in beat (0-1)

    float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
    kickPhase = phasor(kickPhase, kickPitch);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

    float bassPitch = 50.0f; // Hz
    bassPhase = phasor(bassPhase, bassPitch);
    float bass = sinf(bassPhase * TWO_PI); // Sine wave
    bass = tanh(bass * 1.5f); // More saturation
    bass *= 0.2f; // Loudness

    outputBuffer[i] = kick + bass; // Output
  }
  return outputBuffer;
}


float* makeSomeTechno() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR;
    float tBeats = tSeconds * 2.0f; // 120 BPM
    int bar = (int)tBeats / 4;
    float tBeatFrac = tBeats - trunc(tBeats);
    float tSixteenths = tBeats * 4.0f;
    int sixteenth = (int)tSixteenths % 16;
    float tSixteenthFrac = tSixteenths - trunc(tSixteenths);

    float kickPitchEnv = expEnvelope(tBeatFrac, 900.0f, 50.0f);
    float kickPitch = 50.0f + kickPitchEnv;
    kickPhase = phasor(kickPhase, kickPitch);
    float kickEnv = expEnvelope(tBeatFrac, 0.15f, 3.0f);
    float kick = sinf(kickPhase * TWO_PI) * kickEnv;

    float bassPitch = 50.0f;
    bassPhase = phasor(bassPhase, bassPitch);
    float bassEnv = 0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5f);
    float bass = tanh(sinf(bassPhase * TWO_PI) * 1.5f) * bassEnv;

    char chordHit = pattern[sixteenth];
    float chordRootPitch = bassPitch * 4.0f;
    if (bar % 4 > 1) chordRootPitch /= 3.0f / 2.0f;
    chordPhase1 = phasor(chordPhase1, chordRootPitch);
    chordPhase2 = phasor(chordPhase2, chordRootPitch * 6.0f / 5.0f);
    chordPhase3 = phasor(chordPhase3, chordRootPitch * 3.0f / 2.0f);
    float chord1 = saw6f(chordPhase1 * TWO_PI);
    float chord2 = saw6f(chordPhase2 * TWO_PI);
    float chord3 = saw6f(chordPhase3 * TWO_PI);
    float chordEnv = expEnvelope(tSixteenthFrac, 0.1f, 2.0f) * chordHit;
    float chordOut = (chord1 + chord2 + chord3) * chordEnv;
    chordOut = chordOut + processDelay(chordOut, 0.4f, 0.375f) * 0.4f;

    outputBuffer[i] = tanh(kick + bass + chordOut);
  }

  return outputBuffer;
}
