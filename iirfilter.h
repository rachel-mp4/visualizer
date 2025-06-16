#pragma once
#include "soundsource.h"

class IIRFilter : public SoundSource {
public:
  IIRFilter(SoundSource *input, float alpha);
  Sample NextSample() override;

private:
  SoundSource *input;
  Sample state;
  float alpha;
};
