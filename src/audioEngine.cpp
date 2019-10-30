#include "audioEngine.hpp"

#include <iostream>
#include <queue>
#include <complex>
#include <numeric>
#include <time.h>

using namespace std::placeholders;

audioEngine::audioEngine(std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback)
  : output(soundSystem, std::bind(&audioEngine::providerCallback, this, _1)),
    pos(0),
    playbackThread(&audioEngine::playback, this),
    analysisThread(&audioEngine::analysis, this),
    analyzedFrameCallback(analyzedFrameCallback)
{
}

audioEngine::~audioEngine()
{
}

audioProviderFrame audioEngine::providerCallback(int frames)
{
  audioProviderFrame audioFrame = provider.provide(frames);

  for (unsigned i = 0; i < frames; ++i)
    for (unsigned channel = 0; channel < audioSystem::CHANNELS; ++channel)
      if (!analysisQueue[channel].push(audioFrame[channel][i]))
      {
        // i dunno...drop the rest of the frame i guess
      }

  return audioFrame;
}

void audioEngine::analysis()
{
  struct timespec time;
  time.tv_sec = 0;
  time.tv_nsec = 500;

  std::array<std::queue<float>, audioSystem::CHANNELS> window;

  while (true)
  {
    for (unsigned i = 0; i < FRAMES_PER_BUFFER; ++i)
      for (unsigned channel = 0; channel < audioSystem::CHANNELS; ++channel)
      {
        float data;
        while (!analysisQueue[channel].pop(data))
        {
          // TODO std::this_thread::yield
          nanosleep(&time, NULL);
        }
        window[channel].push(data);
      }

    if (std::any_of(window.begin(), window.end(), [](const std::queue<float> &i) 
    { return i.size() < audioSystem::WINDOW_SIZE;}))
      continue; // we don't have enough samples to do FFT yet

    audioSourceFrame sourceSample;
    for (unsigned channel = 0; channel < audioSystem::CHANNELS; ++channel)
    {
      for (unsigned i = 0; i < audioSystem::WINDOW_SIZE; ++i)
      {
        sourceSample[channel][i] = window[channel].front();
        window[channel].pop();
      }
    }

    analysisEngine.analyze(sourceSample);
    analyzedFrameCallback(analysisEngine.getData());
  }
}

void audioEngine::playback()
{
  while (2 > 1)
    soundSystem.waitEvents();
}

const audioAnalyzer::params& audioEngine::getParams() const
{
  return analysisEngine.getParams();
}

void audioEngine::setParams(const audioAnalyzer::params& newParams)
{
  analysisEngine.setParams(newParams);
}
