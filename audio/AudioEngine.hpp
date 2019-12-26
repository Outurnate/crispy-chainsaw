#ifndef AUDIOENGINE_HPP
#define AUDIOENGINE_HPP

#include <memory>
#include <thread>
#include <array>
#include <functional>
#include <list>
#include <chrono>
#include <boost/lockfree/spsc_queue.hpp>

#include "AudioAnalyzer.hpp"
#include "AudioProvider.hpp"
#include "AudioOutput.hpp"
#include "SoundIO.hpp"

class AudioEngine
{
public:
  struct Params
  {
    float alpha = 0.2f;
    float gamma = 2.0f;
    float scale = 1.0f / 10.0f;
    float exponent = 1.0f;
  };

  AudioEngine(std::function<void(const FFTSpectrumData&)> AnalyzedFrameCallback);

  const Params& getParams() const;
  void setParams(const Params& newParams);
private:
  typedef std::chrono::steady_clock clock;

  void analysis();
  void playback();
  void audioPlayed(const AudioProviderFrame& frame, std::chrono::duration<double> latency);
  void processDelayedFrames();

  SoundIO::System soundSystem;
  std::list<std::tuple<std::chrono::duration<double>, StereoPair<std::vector<float> > > > delayedFrames;
  AudioProvider provider;
  AudioOutput output;
  std::thread playbackThread;
  std::thread analysisThread;
  boost::lockfree::spsc_queue<StereoSample, boost::lockfree::capacity<AudioSystem::WINDOW_SIZE * 8> > analysisQueue; // 8 is completely arbitrary
  AudioAnalyzer analysisEngine;
  std::function<void(const FFTSpectrumData&)> analyzedFrameCallback;
  Params currentParams;
};

#endif
