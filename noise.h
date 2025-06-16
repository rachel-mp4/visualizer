#pragma once
#include "soundsource.h"
#include <random>

class Noise : public SoundSource {
public:
  Noise();
  Sample NextSample() override;

private:
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist;
};
