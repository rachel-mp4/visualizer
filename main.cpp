#include "iirfilter.h"
#include "noise.h"
#include "sample.h"
#include "sample_buffer.h"
#include "sawwave.h"
#include <deque>
#include <iostream>
#include <portaudio.h>
#include <raylib.h>
#include <sys/_types/_u_int32_t.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

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

#define AMPLITUDE 0.2f
#define SAMPLE_RATE 44100
#define TAU 6.2831855f

SampleBuffer visBuffer;

bool usingFile = false;
struct playbackData {
  std::vector<Sample> samples;
  size_t frameIndex;
};
playbackData pbd;

SawWave wave = SawWave(Sample(440.1, 439.9), SAMPLE_RATE);
SawWave wave2 = SawWave(Sample(220.4, 218.6), SAMPLE_RATE);
Noise n = Noise();
IIRFilter iir = IIRFilter(&wave2, 0.1f);

static int audioCallback(const void *input, void *output,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  float *out = static_cast<float *>(output);
  for (unsigned int i = 0; i < framesPerBuffer; ++i) {
    Sample s = Sample(0.0f);
    if (usingFile) {
      if (pbd.frameIndex < pbd.samples.size()) {
        s = pbd.samples[pbd.frameIndex];
        pbd.frameIndex += 1;
      }
    } else {
      s = s + (iir.NextSample()) * AMPLITUDE;
    }
    visBuffer.push(s);
    *out++ = s.left;
    *out++ = s.right;
  }
  return paContinue;
}

void vwindow(SampleBuffer &vb) {
  const int screenwidth = 800;
  const int screenheight = 800;
  InitWindow(screenwidth, screenheight, "oscilloscope");
  SetTargetFPS(144);

  std::deque<Sample> smoothed(1600, Sample(0.0));
  std::deque<Sample> resampled(800, Sample(0.0));
  float alpha = 0.05f;
  Sample last = Sample(0.0f);
  Sample curresample = Sample(0.0f);
  int ninresample = 0;
  Font inter = LoadFontEx("font/Inter-ExtraLight.ttf", 256, 0, 0);
  Font interItalic = LoadFontEx("font/Inter-ExtraLightItalic.ttf", 256, 0, 0);
  // Color fg = {37, 22, 1, 255};
  Color fg = WHITE;
  // Color bg = {255, 237, 201, 255};
  Color bg = BLACK;

  while (!WindowShouldClose()) {
    Sample out;
    while (vb.pop(out)) {
      last = out * alpha + last * (1.0f - alpha);
      smoothed.pop_front();
      smoothed.push_back(last * 2.0f);
      curresample = curresample + last * .25f;
      ninresample += 1;
      if (ninresample >= 4) {
        resampled.pop_front();
        resampled.push_back(curresample);
        curresample = Sample(0.0f);
        ninresample = 0;
      }
    }
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
    float len = 700 - MeasureTextEx(inter, "", 32.0f, 0.0f).x;
    DrawTextEx(inter, "", {len, 68}, 32.0f, 0.0f, fg);
    DrawTextPro(interItalic, "", {68, 700}, {0, 0}, -90, 32.0f, 0.0f, fg);

    for (int i = resampled.size() - 1; i > 0; i--) {
      Sample r1 = resampled[i];
      r1 = Sample(2 * r1.left - 2 * r1.right, r1.left + r1.right);
      Sample r2 = resampled[i - 1];
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
    usingFile = true;
    const char *filepath = argv[1];
    std::cout << "loading file" << std::endl;
    uint32_t sr = 44100;
    uint16_t cs = 2;
    std::vector<Sample> samples = loadWavFile(filepath, sr, cs);
    pbd = {samples, 0};
  }

  PaError err;
  err = Pa_Initialize();
  if (err != paNoError)
    return 1;

  PaStream *stream;
  err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, 256,
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
