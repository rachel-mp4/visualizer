#include "sawwave.h"

SawWave::SawWave(Sample freq, float sampleRate)
    : frequency(freq), sampleRate(sampleRate), phase(Sample(0.0f)) {}

Sample SawWave::NextSample() {
  Sample sample = phase;
  phase += 2.0f * frequency / sampleRate;
  if (phase.left > 1.f)
    phase.left -= 2.f;
  if (phase.right > 1.f)
    phase.right -= 2.f;
  return sample;
}
