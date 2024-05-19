#include <math.h>
#include <stdlib.h>

#define SAMPLERATE 48000.0f
#define SAMPLE_DUR 1.0f / SAMPLERATE
#define TWO_PI     6.283185307179586f

float outputBuffer[128];

int currentStep = -1;

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
  return powf(1.0f - t, exp) * powf(2.0f, -0.001f / t) * amp;
}

float saw6f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0f) * 0.5f +
    sinf(phase * 3.0f) * 0.33f +
    sinf(phase * 4.0f) * 0.25f +
    sinf(phase * 5.0f) * 0.2f +
    sinf(phase * 6.0f) * 0.16f;
}


// Ring buffer delay line
float delayLine[(int)SAMPLERATE] = {0}; // Store max 1 second of samples
int delayIndex = 0;
float processDelayNoFb(float input, float delayTime) {
  delayLine[delayIndex] = input; // Write current input
  delayIndex++; // Move to next sample
  delayIndex %= (int)(delayTime * SAMPLERATE); // Wrap if exceeding delay time
  return delayLine[delayIndex]; // Read delayed output
}

float processDelay(float input, float feedback, float delayTime) {
  // Write current input retaining some of the previous output as feedback
  delayLine[delayIndex] = input + delayLine[delayIndex] * feedback;
  delayIndex++; // Move to next sample
  delayIndex %= (int)(delayTime * SAMPLERATE); // Wrap if exceeding delay time
  return delayLine[delayIndex]; // Read delayed output
}

float saw2f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0f) * 0.5f;
}
float saw3f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0f) * 0.5f +
    sinf(phase * 3.0f) * 0.33f;
}
float saw4f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0f) * 0.5f +
    sinf(phase * 3.0f) * 0.33f +
    sinf(phase * 4.0f) * 0.25f;
}
float saw5f(float phase) {
  return sinf(phase) +
    sinf(phase * 2.0f) * 0.5f +
    sinf(phase * 3.0f) * 0.33f +
    sinf(phase * 4.0f) * 0.25f +
    sinf(phase * 5.0f) * 0.2f;
}

void setStep(int step) {
  currentStep = step;
  if (step == 20 || step == 21 || step == 22) {
    tSamples = 0; // hear the "chord progression" from the beginning
  }
}

float* makeSomeTechno() {
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR;
    float tBeats = tSeconds * 2.0f; // 120 BPM
    int bar = tBeats / 4.0f; // Current bar (in 4/4 time)
    float tBeatFrac = tBeats - trunc(tBeats);
    float tSixteenths = tBeats * 4.0f;
    int sixteenth = (int)tSixteenths % 16;
    float tSixteenthFrac = tSixteenths - trunc(tSixteenths);

    if (currentStep == 0) {
      float kickPitch = 200.0f;
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sinf(kickPhase * TWO_PI) * 0.15f;
      outputBuffer[i] = kick;
    } else if (currentStep == 1) {
      float kickPitch = 200.0f; // Hz
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= linearEnvelope(tBeatFrac, 0.15f); // Shape the amplitude
      outputBuffer[i] = kick; // Output
    } else if (currentStep == 2) {
      float kickPitch = 200.0f; // Hz
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude
      outputBuffer[i] = kick; // Output
    } else if (currentStep == 3) {
      float kickPitchEnv = expEnvelope(tBeatFrac, 200.0f, 3.0);
      float kickPitch = 50.0 + kickPitchEnv; // Hz
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude
      outputBuffer[i] = kick; // Output
    } else if (currentStep == 4) {
      float kickPitchEnv = expEnvelope(tBeatFrac, 900.0f, 50.0);
      float kickPitch = 50.0 + kickPitchEnv; // Hz
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude
      outputBuffer[i] = kick; // Output
    } else if (currentStep == 5) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass *= 0.2f; // Loudness

      outputBuffer[i] = kick + bass; // Output
    } else if (currentStep == 6) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass); // Saturation
      bass *= 0.2f; // Loudness

      outputBuffer[i] = kick + bass; // Output
    } else if (currentStep == 7) {
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
    } else if (currentStep == 8) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 3.0)); // "Sidechain"

      outputBuffer[i] = kick + bass; // Output
    } else if (currentStep == 9) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      outputBuffer[i] = kick + bass; // Output
    } else if (currentStep == 10) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = sinf(chordPhase1 * TWO_PI);
      chord *= expEnvelope(tBeatFrac, 0.1, 3.0);

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 11) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = sinf(chordPhase1 * TWO_PI);
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0);

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 12) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = sinf(chordPhase1 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 13) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = saw2f(chordPhase1 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 14) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = saw3f(chordPhase1 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 15) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = saw4f(chordPhase1 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 16) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = saw5f(chordPhase1 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 17) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordPitch);
      float chord = saw6f(chordPhase1 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 18) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordRootPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordRootPitch);
      chordPhase2 = phasor(chordPhase2, chordRootPitch * 3.0f/2.0f);
      float chord =
        saw6f(chordPhase1 * TWO_PI) +
        saw6f(chordPhase2 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 19) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordRootPitch = bassPitch * 4.0f;
      chordPhase1 = phasor(chordPhase1, chordRootPitch);
      chordPhase2 = phasor(chordPhase2, chordRootPitch * 3.0f/2.0f);
      chordPhase3 = phasor(chordPhase3, chordRootPitch * 6.0f/5.0f);
      float chord =
        saw6f(chordPhase1 * TWO_PI) +
        saw6f(chordPhase2 * TWO_PI) +
        saw6f(chordPhase3 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 20) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordRootPitch = bassPitch * 4.0f;
      if (bar % 4 == 2 || bar % 4 == 3) chordRootPitch *= 2.0f/3.0f; // Perfect 5th down
      chordPhase1 = phasor(chordPhase1, chordRootPitch);
      chordPhase2 = phasor(chordPhase2, chordRootPitch * 3.0f/2.0f);
      chordPhase3 = phasor(chordPhase3, chordRootPitch * 6.0f/5.0f);
      float chord =
        saw6f(chordPhase1 * TWO_PI) +
        saw6f(chordPhase2 * TWO_PI) +
        saw6f(chordPhase3 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 21) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordRootPitch = bassPitch * 4.0f;
      if (bar % 4 == 2 || bar % 4 == 3) chordRootPitch *= 2.0f/3.0f; // Perfect 5th down
      chordPhase1 = phasor(chordPhase1, chordRootPitch);
      chordPhase2 = phasor(chordPhase2, chordRootPitch * 3.0f/2.0f);
      chordPhase3 = phasor(chordPhase3, chordRootPitch * 6.0f/5.0f);
      float chord =
        saw6f(chordPhase1 * TWO_PI) +
        saw6f(chordPhase2 * TWO_PI) +
        saw6f(chordPhase3 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;
      chord = chord + processDelayNoFb(chord, 0.375f) * 0.4; // Dotted 8th delay

      outputBuffer[i] = kick + bass + chord; // Output
    } else if (currentStep == 22 || currentStep == 999) {
      float kickPitch = 50.0f + expEnvelope(tBeatFrac, 900.0f, 50.0f); // More!
      kickPhase = phasor(kickPhase, kickPitch);
      float kick = sin(kickPhase * TWO_PI); // Sine wave
      kick *= expEnvelope(tBeatFrac, 0.15f, 3.0); // Shape the amplitude

      float bassPitch = 50.0f; // Hz
      bassPhase = phasor(bassPhase, bassPitch);
      float bass = sinf(bassPhase * TWO_PI); // Sine wave
      bass = tanh(bass * 1.5f); // More saturation
      bass *= (0.2f - expEnvelope(tBeatFrac, 0.2f, 0.5)); // "Sidechain"

      // Chords
      float chordRootPitch = bassPitch * 4.0f;
      if (bar % 4 == 2 || bar % 4 == 3) chordRootPitch *= 2.0f/3.0f; // Perfect 5th down
      chordPhase1 = phasor(chordPhase1, chordRootPitch);
      chordPhase2 = phasor(chordPhase2, chordRootPitch * 3.0f/2.0f);
      chordPhase3 = phasor(chordPhase3, chordRootPitch * 6.0f/5.0f);
      float chord =
        saw6f(chordPhase1 * TWO_PI) +
        saw6f(chordPhase2 * TWO_PI) +
        saw6f(chordPhase3 * TWO_PI);
      float chordHit = pattern[(int)tSixteenths % 16];
      chord *= expEnvelope(tSixteenthFrac, 0.1, 3.0) * chordHit;
      chord = chord + processDelay(chord, 0.5f, 0.375f) * 0.4; // Dotted 8th delay

      outputBuffer[i] = kick + bass + chord; // Output
    }
  }

  return outputBuffer;
}
