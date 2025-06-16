#pragma once
#include "soundsource.h"
#include <cmath>

class SawWave : public SoundSource {
public:
  SawWave(Sample freq, float sampleRate);
  Sample NextSample() override;

private:
  Sample frequency;
  float sampleRate;
  Sample phase;
};
