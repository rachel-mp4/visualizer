#pragma once
#include "sample.h"

class SoundSource {
public:
  virtual Sample NextSample() = 0;
  virtual ~SoundSource() {}
};
