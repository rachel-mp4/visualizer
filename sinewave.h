#pragma once
#include "soundsource.h"
#include <cmath>

class SineWave : public SoundSource {
public:
  SineWave(Sample freq, float sampleRate);
  Sample NextSample() override;

private:
  Sample frequency;
  float sampleRate;
  Sample phase;
};
