#include <math.h>
#include <stdlib.h>

// Determine duration of one sample
#define SAMPLERATE 48000.0f
#define SAMPLE_DUR 1.0f / SAMPLERATE // ~0.02ms
#define TWO_PI 6.2831853f

// Define a phasor that runs from 0 to 1 at a given frequency
float phasor(float phase, float freq) {
  phase += freq * SAMPLE_DUR; // Increment by one sample
  phase -= trunc(phase);      // When it reaches 1 go back to 0
  return phase;
}

// Additive sawtooth wave
float saw6f(float phase) {
  // Add harmonics
  return sinf(phase)
    + sinf(phase * 2.0f) * 0.5f
    + sinf(phase * 3.0f) * 0.33f
    + sinf(phase * 4.0f) * 0.25f
    + sinf(phase * 5.0f) * 0.2f
    + sinf(phase * 6.0f) * 0.16f
      ;
}

float envelope(float t, float amp, float exp) {
  float env = powf(1.0 - t, exp); // Exponentially decay from 1 to 0
  env *= amp; // Decay from amp to 0
  return env;
}

// Rhythm pattern in sixteenth notes
// 1 = play chord, 0 = don't play chord
const char chordPattern[16] = {
  0, 1, 0, 0,
  0, 0, 1, 0,
  1, 0, 0, 1,
  0, 0, 1, 0
};

// Ring buffer delay line
float delayLine[(int)SAMPLERATE] = {0}; // Store max 1 second of samples
int delayIndex = 0;
float processDelay(float input, float feedback, float delayTime) {
  // Write current input retaining some of the previous output as feedback
  delayLine[delayIndex] = input + delayLine[delayIndex] * feedback;
  delayIndex++; // Move to next sample
  delayIndex %= (int)(delayTime * SAMPLERATE); // Wrap if exceeding delay time
  return delayLine[delayIndex]; // Read delayed output
}

static float outputBuffer[128]; // Array of samples

unsigned long long tSamples = 0; // Accumulated samples
float kickPhase = 0.0f, bassPhase = 0.0f, // Accumulate phase over time
      chordPhase1 = 0.0f, chordPhase2 = 0.0f, chordPhase3 = 0.0f;

float* makeSomeTechno() {
  // Process 128 samples into the array
  for (int i = 0; i < 128; i++) {
    float tSeconds = tSamples++ * SAMPLE_DUR; // Current time
    float tBeats = tSeconds * 2.0f; // Current beat @ 120 BPM
    float tBeatFrac = tBeats - trunc(tBeats); // Time within beat (0-1)
    int bar = tBeats / 4.0f; // Current bar (in 4/4 time)
    float tSixteenths = tBeats * 4.0; // Current 16th note
    float tSixteenthFrac = tSixteenths - trunc(tSixteenths); // Time within 16th (0-1)

    // Kick
    float kickFreq = 50.0f + envelope(tBeatFrac, 900.0f, 50.0f); // Freq decay 950->50Hz
    kickPhase = phasor(kickPhase, kickFreq);
    float kick = sin(kickPhase * TWO_PI); // Sine wave
    kick *= envelope(tBeatFrac, 0.15f, 3.0f); // Shape the amplitude

    // Bass
    float bassFreq = 50.0f; // Hz
    bassPhase = phasor(bassPhase, bassFreq);
    float bass = sinf(bassPhase * TWO_PI); // Sine wave
    bass = tanh(bass * 1.5f); // More saturation
    bass *= (0.2f - envelope(tBeatFrac, 0.2f, 0.5f)); // "Sidechaining"

    // Chords
    float chordFreq = bassFreq * 4.0f; // 200Hz, 2 octaves above bass
    if (bar % 4 == 2 || bar % 4 == 3) chordFreq *= 2.0f/3.0f; // Perfect 5th down
    chordPhase1 = phasor(chordPhase1, chordFreq);
    chordPhase2 = phasor(chordPhase2, chordFreq * 3.0f/2.0f); // Perfect 5th
    chordPhase3 = phasor(chordPhase3, chordFreq * 6.0f/5.0f); // Minor 3rd
    float chord =
      saw6f(chordPhase1 * TWO_PI) +
      saw6f(chordPhase2 * TWO_PI) +
      saw6f(chordPhase3 * TWO_PI);
    int chordPatternIndex = (int)tSixteenths % 16; // 0-15
    char chordHit = chordPattern[chordPatternIndex]; // 0 or 1 if chord should play
    chord *= envelope(tSixteenthFrac, 0.1f, 3.0f) * chordHit;
    chord = chord + processDelay(chord, 0.5f, 0.375f) * 0.4f; // Dotted 8th feedback delay

    outputBuffer[i] = kick + bass + chord; // Output
  }
  // Return pointer to the array
  return outputBuffer;
}