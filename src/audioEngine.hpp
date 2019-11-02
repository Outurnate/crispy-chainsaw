#ifndef AUDIOENGINE_HPP
#define AUDIOENGINE_HPP

#include <memory>
#include <thread>
#include <array>
#include <functional>
#include <list>
#include <chrono>
#include <boost/lockfree/spsc_queue.hpp>

#include "audioAnalyzer.hpp"
#include "audioProvider.hpp"
#include "audioOutput.hpp"
#include "soundIO.hpp"

class audioEngine
{
public:
  struct params
  {
    float alpha = 0.2f;
    float gamma = 2.0f;
    float scale = 1.0f / 10.0f;
    float exponent = 1.0f;
  };

  audioEngine(std::function<void(const fftSpectrumData&)> analyzedFrameCallback);
  virtual ~audioEngine();

  const params& getParams() const;
  void setParams(const params& newParams);
private:
  typedef std::chrono::steady_clock clock;

  void analysis();
  void playback();
  void audioPlayed(const audioProviderFrame& frame, std::chrono::duration<double> latency);
  void processDelayedFrames();

  soundio::system soundSystem;
  std::list<std::tuple<std::chrono::duration<double>, stereoPair<std::vector<float> > > > delayedFrames;
  audioProvider provider;
  audioOutput output;
  std::thread playbackThread;
  std::thread analysisThread;
  boost::lockfree::spsc_queue<stereoSample, boost::lockfree::capacity<audioSystem::WINDOW_SIZE * 8> > analysisQueue; // 8 is completely arbitrary
  audioAnalyzer analysisEngine;
  std::function<void(const fftSpectrumData&)> analyzedFrameCallback;
  params currentParams;
};

#endif
