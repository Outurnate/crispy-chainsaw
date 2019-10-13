#ifndef AUDIOENGINE_HPP
#define AUDIOENGINE_HPP

#include <memory>
#include <thread>
#include <array>
#include <functional>
#include <boost/lockfree/spsc_queue.hpp>
#include <portaudiocpp/PortAudioCpp.hxx>
#include <AudioFile.h>

#include "audioAnalyzer.hpp"

class audioEngine : private portaudio::AutoSystem
{
public:
  typedef audioAnalyzer analyzer;

  audioEngine(std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback);
  virtual ~audioEngine();

  void loadFile(const std::string &fileURI);
  void start();
  void stop();
  const bool isPlaying() const;
  const audioAnalyzer::params& getParams() const;
  void setParams(const audioAnalyzer::params& newParams);
private:
  static constexpr unsigned FRAMES_PER_BUFFER = 64;

  int getSample(const void *inputBuffer, void *outputBuffer,
      unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
      PaStreamCallbackFlags statusFlags);
  void analysis();

  AudioFile<double> file;
  std::unique_ptr<portaudio::MemFunCallbackStream<audioEngine> > stream;

  unsigned long pos;
  std::thread analysisThread;
  std::array<
      boost::lockfree::spsc_queue<float,
          boost::lockfree::capacity<FRAMES_PER_BUFFER * 32> >, audioSystem::CHANNELS> analysisQueue; // 32 is completely arbitrary
  analyzer analysisEngine;
  std::function<void(const audioAnalyzedFrame&)> analyzedFrameCallback;
};

#endif
