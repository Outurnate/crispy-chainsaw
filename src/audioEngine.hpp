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
  audioEngine(std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback);
  virtual ~audioEngine();

  const audioAnalyzer::params& getParams() const;
  void setParams(const audioAnalyzer::params& newParams);
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
  std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback;
};

#endif
