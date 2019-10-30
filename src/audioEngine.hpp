#ifndef AUDIOENGINE_HPP
#define AUDIOENGINE_HPP

#include <memory>
#include <thread>
#include <array>
#include <functional>
#include <boost/lockfree/spsc_queue.hpp>

#include "audioAnalyzer.hpp"
#include "audioProvider.hpp"
#include "audioOutput.hpp"
#include "soundIO.hpp"

class audioEngine
{
public:
  typedef audioAnalyzer analyzer;

  audioEngine(std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback);
  virtual ~audioEngine();

  const audioAnalyzer::params& getParams() const;
  void setParams(const audioAnalyzer::params& newParams);
private:
  static constexpr unsigned FRAMES_PER_BUFFER = 64;

  void analysis();
  void playback();
  audioProviderFrame providerCallback(int frames);

  soundio::system soundSystem;
  audioProvider provider;
  audioOutput output;
  std::thread playbackThread;
  std::thread analysisThread;
  unsigned long pos;
  std::array<
      boost::lockfree::spsc_queue<float,
          boost::lockfree::capacity<FRAMES_PER_BUFFER * 32> >, audioSystem::CHANNELS> analysisQueue; // 32 is completely arbitrary
  analyzer analysisEngine;
  std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback;
};

#endif
