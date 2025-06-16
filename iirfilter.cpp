#include "iirfilter.h"

IIRFilter::IIRFilter(SoundSource *src, float alpha)
    : input(src), state(Sample(0.0f)), alpha(alpha) {}

Sample IIRFilter::NextSample() {
  Sample current = input->NextSample();
  state = alpha * current + (1.0f - alpha) * state;
  return state;
}
