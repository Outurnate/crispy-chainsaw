#include "audioEngine.hpp"

#include <iostream>
#include <queue>
#include <complex>
#include <numeric>
#include <spdlog/spdlog.h>

using namespace std::placeholders;

audioEngine::audioEngine(std::function<void(const fftSpectrumData&)> analyzedFrameCallback)
  : delayedFrames(),
    provider(soundSystem),
    output(soundSystem, std::bind(&audioProvider::provide, &provider, _1), std::bind(&audioEngine::audioPlayed, this, _1, _2)),
    playbackThread(&audioEngine::playback, this),
    analysisThread(&audioEngine::analysis, this),
    analyzedFrameCallback(analyzedFrameCallback)
{
  output.start();
}

audioEngine::~audioEngine()
{
}

const audioEngine::params& audioEngine::getParams() const
{
  return currentParams;
}

void audioEngine::setParams(const audioEngine::params& newParams)
{
  currentParams = newParams;
}

void audioEngine::playback()
{
  while (2 > 1)
  {
    soundSystem.flushEvents();
    processDelayedFrames();
  }
}

/////////////////////////////////////////////
//            Playback Thread              //
/////////////////////////////////////////////

void audioEngine::audioPlayed(const audioProviderFrame& frame, std::chrono::duration<double> latency)
{
  stereoPair<std::vector<float> > copiedFrame;
  copiedFrame.left  = std::vector<float>(frame.left.begin(),  frame.left.end());
  copiedFrame.right = std::vector<float>(frame.right.begin(), frame.right.end());
  delayedFrames.emplace_front(clock::now().time_since_epoch() + latency, copiedFrame);
}

void audioEngine::processDelayedFrames()
{
  auto current = delayedFrames.begin();
  while (current != delayedFrames.end())
  {
    auto& [latency, frame] = *current;

    // when the latency hits zero, copy this chunk of
    // samples from thread local delayedFrames to spsc
    // queue
    if (latency < clock::now().time_since_epoch())
    {
      for (unsigned i = 0; i < frame.left.size(); ++i)
      {
        stereoSample sample;
        sample.left  = frame.left[i];
        sample.right = frame.right[i];
        if (!analysisQueue.push(sample))
        {
          // couldn't push the sample, analysis thread running behind
          spdlog::warn("Could not push sample; dropping.  Analysis thread is running behind");
        }
      }
      current = delayedFrames.erase(current);
    }
    else
      ++current;
  }
}

/////////////////////////////////////////////
//            Analysis Thread              //
/////////////////////////////////////////////

void audioEngine::analysis()
{
  audioSourceFrame analysisSamples;
  unsigned samplesReadyForAnalysis;
  std::array<stereoSample, audioSystem::WINDOW_SIZE> popped;

  while (2 > 1)
  {
    samplesReadyForAnalysis = 0;

    while (analysisQueue.read_available() > audioSystem::WINDOW_SIZE)
    {
      analysisQueue.pop(popped.data(), popped.size());

      if (samplesReadyForAnalysis == 0)
        for (unsigned i = 0; i < audioSystem::WINDOW_SIZE; ++i)
        {
          analysisSamples.left[i]  = popped[i].left;
          analysisSamples.right[i] = popped[i].right;
        }

      ++samplesReadyForAnalysis;
    }
    if (samplesReadyForAnalysis != 0)
      analyzedFrameCallback(analysisEngine.analyze(analysisSamples, currentParams.alpha, currentParams.gamma, currentParams.scale, currentParams.exponent));
    if (samplesReadyForAnalysis > 1)
      spdlog::warn("{} samples read.  Analysis thread is running behind", samplesReadyForAnalysis);
  }
}
