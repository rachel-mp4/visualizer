#pragma once
#include "soundsource.h"

class ADSRFilter : public SoundSource {
public:
  ADSRFilter(SoundSource *input, float attack, float decay, float sustain,
             float release, float samplerate);
  Sample NextSample() override;
  void Play();

private:
  SoundSource *input;
  float attack;
  float decay;
  float sustain;
  float release;
  float time;
  float samplerate;
};
