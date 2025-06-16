#include "sample.h"
#include "sample_buffer.h"
#include <deque>
#include <iostream>
#include <portaudio.h>
#include <raylib.h>
#include <sys/_types/_u_int32_t.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

// loads the wav file!
std::vector<Sample> loadWavFile(const char *filename, uint32_t &sampleRate,
                                uint16_t &channels) {
  drwav wav;
  if (!drwav_init_file(&wav, filename, NULL)) {
    throw std::runtime_error("failed to open WAV file");
  }
  sampleRate = wav.sampleRate;
  channels = wav.channels;
  size_t totalSampleCount = wav.totalPCMFrameCount * wav.channels;
  std::vector<float> pcmData(totalSampleCount);
  drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, pcmData.data());
  std::cout << "opened! now sampling" << std::endl;
  std::vector<Sample> samples(wav.totalPCMFrameCount);
  for (int i = 0; i < wav.totalPCMFrameCount; i += wav.channels) {
    Sample s;
    if (channels == 1) {
      s = Sample(pcmData[i]);
    } else {
      s = Sample(pcmData[2 * i], pcmData[2 * i + 1]);
    }
    samples[i] = s;
  }
  drwav_uninit(&wav);
  std::cout << "ok!" << std::endl;
  return samples;
}

// does the playback and forwards the raw audio samples to the visualizer
SampleBuffer visBuffer;
struct playbackData {
  std::vector<Sample> samples;
  size_t frameIndex;
};
playbackData pbd;

// actual playback stuff
static int audioCallback(const void *input, void *output,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  float *out = static_cast<float *>(output);
  for (unsigned int i = 0; i < framesPerBuffer; ++i) {
    Sample s = Sample(0.0f);
    if (pbd.frameIndex < pbd.samples.size()) {
      s = pbd.samples[pbd.frameIndex];
      pbd.frameIndex += 1;
    }
    visBuffer.push(s);
    *out++ = s.left;
    *out++ = s.right;
  }
  return paContinue;
}

// everything visual
void vwindow(SampleBuffer &vb) {
  const int screenwidth = 800;
  const int screenheight = 800;
  InitWindow(screenwidth, screenheight, "oscilloscope");
  SetTargetFPS(144);
  Color bg = BLACK;
  Color fg = WHITE;

  // basically, audio samples are very noisy because there are lots of
  // frequencies involved. an x-y oscilloscope plots the points on the x-y
  // plane, however the very high frequencies will shift from positive to
  // negative basically each sample, which causes the oscilloscope to always get
  // plotted back and forth through the origin (x:0, y:0). this isn't very
  // #aesthetic, so we want to smooth out the samples so that it needs to spend
  // some time at the very positive and very negative points before it will get
  // pulled in that way, minimizing the amount it passes through the origin!
  std::deque<Sample> smoothed(1600, Sample(0.0));
  std::deque<Sample> resampled(800, Sample(0.0));
  float alpha =
      0.05f; // a parameter to help smooth out the samples. PLAY WITH IT!
  Sample last = Sample(0.0f); // the way we smooth is by smearing the current
                              // sample into a buffer (called last)
  Sample curresample =
      Sample(0.0f); // after i smooth the points, i resample them, so each point
                    // becomes the average of maxinresample points
  int ninresample = 0; // how many are currently added to our curresample buffer
  int maxinresample = 4; // how many points get resampled. PLAY WITH IT!

  while (!WindowShouldClose()) {

    // this is the smearing logic, you can totally mess with this!
    Sample out;
    while (vb.pop(out)) {
      last = out * alpha + last * (1.0f - alpha);
      smoothed.pop_front();
      smoothed.push_back(last);
      curresample = curresample + last / maxinresample;
      ninresample += 1;
      if (ninresample >= maxinresample) {
        resampled.pop_front();
        resampled.push_back(curresample);
        curresample = Sample(0.0f);
        ninresample = 0;
      }
    }

    // of course, here is where the actual drawing happens!
    // look up the raylib's cheatsheet for anything more complex!
    // https://www.raylib.com/cheatsheet/cheatsheet.html
    BeginDrawing();
    ClearBackground(bg);
    Vector2 tl = {100, 100};
    Vector2 tr = {100, 700};
    Vector2 bl = {700, 100};
    Vector2 br = {700, 700};
    DrawLineEx(tl, tr, 1.0f, fg);
    DrawLineEx(tr, br, 1.0f, fg);
    DrawLineEx(br, bl, 1.0f, fg);
    DrawLineEx(bl, tl, 1.0f, fg);

    for (int i = resampled.size() - 1; i > 0; i--) {
      Sample r1 = resampled[i] * 2.0f;
      r1 = Sample(2 * r1.left - 2 * r1.right, r1.left + r1.right);
      Sample r2 = resampled[i - 1] * 2.0f;
      r2 = Sample(2 * r2.left - 2 * r2.right, r2.left + r2.right);
      Sample s1 = screenheight * (r1 / 2.0f + Sample(0.5f));
      Vector2 v1 = {s1.left, s1.right};
      Sample s2 = screenheight * (r2 / 2.0f + Sample(0.5f));
      Vector2 v2 = {s2.left, s2.right};
      float f = 1.0f - ((float)i / resampled.size());
      DrawLineEx(v1, v2, 1.0f, fg);
    }
    EndDrawing();
  }
  CloseWindow();
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    const char *filepath = argv[1];
    std::cout << "loading file" << std::endl;
    uint32_t sr = 44100;
    uint16_t cs = 2;
    std::vector<Sample> samples = loadWavFile(filepath, sr, cs);
    pbd = {samples, 0};
  } else {
    return 0;
  }

  PaError err;
  err = Pa_Initialize();
  if (err != paNoError)
    return 1;

  PaStream *stream;
  err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256,
                             audioCallback, nullptr);
  if (err != paNoError)
    return 1;

  Pa_StartStream(stream);
  vwindow(visBuffer);
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}
