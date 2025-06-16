#include "noise.h"
#include <chrono>

Noise::Noise()
    : rng(std::chrono::steady_clock::now().time_since_epoch().count()),
      dist(-1.0f, 1.0f) {}

Sample Noise::NextSample() { return Sample(dist(rng), dist(rng)); }
