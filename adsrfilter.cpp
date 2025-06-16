#include "adsrfilter.h"

ADSRFilter::ADSRFilter(SoundSource *src, float attack, float decay,
                       float sustain, float release, float samplerate)
    : input(src), attack(attack), decay(decay), sustain(sustain),
      release(release), time(0.0f), samplerate(samplerate) {}

Sample ADSRFilter::NextSample() {
  Sample current = input->NextSample();
  time += 1.0f / samplerate;
  time += return state;
}
