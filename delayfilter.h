#pragma once
#include "soundsource.h"
#include <vector>

class DelayFilter : public SoundSource {
public:
  DelayFilter(SoundSource *input, int samples, float decay);
  Sample NextSample() override;

private:
  SoundSource *input;
  std::vector<Sample> samples;
  float decay;
  int head;
  int len;
};
