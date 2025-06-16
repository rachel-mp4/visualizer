#include "sinewave.h"

SineWave::SineWave(Sample freq, float sampleRate)
    : frequency(freq), sampleRate(sampleRate), phase(Sample(0.0f)) {}

Sample SineWave::NextSample() {
  Sample sample = Sample(std::sinf(phase.left), std::sinf(phase.right));
  phase += 6.28318f * frequency / sampleRate;
  if (phase.left > 6.28318f)
    phase.left -= 6.28318f;
  if (phase.right > 6.28318f)
    phase.right -= 6.28318f;
  return sample;
}
