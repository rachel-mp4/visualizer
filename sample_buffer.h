#ifndef SAMPLE_BUFFER_H
#define SAMPLE_BUFFER_H

#include "sample.h"
#include <atomic>
#include <vector>

class SampleBuffer {
  std::vector<Sample> buffer;
  std::atomic<size_t> writeIndex;
  std::atomic<size_t> readIndex;
  size_t bufferSize;

public:
  SampleBuffer(size_t size = 2048);
  void push(const Sample &s);
  bool pop(Sample &out);
  void reset();
};

#endif
