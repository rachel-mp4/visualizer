#include "delayfilter.h"

DelayFilter::DelayFilter(SoundSource *src, int samples, float decay)
    : input(src), samples(std::vector<Sample>(samples)), decay(decay), head(0),
      len(samples) {}

Sample DelayFilter::NextSample() {
  Sample current = input->NextSample();
  Sample state = current + samples[head] * decay;
  samples[head] = state;
  head = (head + 1) % len;
  return state;
}
