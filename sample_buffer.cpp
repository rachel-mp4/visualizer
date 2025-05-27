#include "sample_buffer.h"
#include <atomic>

SampleBuffer::SampleBuffer(size_t size)
    : buffer(size), writeIndex(0), readIndex(0), bufferSize(size) {}

void SampleBuffer::push(const Sample &s) {
  size_t i = writeIndex.fetch_add(1, std::memory_order_relaxed) % bufferSize;
  buffer[i] = s;
}

bool SampleBuffer::pop(Sample &out) {
  size_t r = readIndex.load(std::memory_order_relaxed);
  size_t w = writeIndex.load(std::memory_order_acquire);
  if (r == w)
    return false;

  out = buffer[r % bufferSize];
  readIndex.store(r + 1, std::memory_order_release);
  return true;
}

void SampleBuffer::reset() {
  writeIndex.store(0);
  readIndex.store(0);
}
