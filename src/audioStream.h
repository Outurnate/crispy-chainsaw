#ifndef AUDIOSTREAM_H_
#define AUDIOSTREAM_H_

#include <memory>
#include <thread>
#include <array>
#include <boost/circular_buffer.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <portaudiocpp/PortAudioCpp.hxx>
#include <AudioFile.h>

#include "audioAnalyzer.h"

class audioStream : private portaudio::AutoSystem
{
public:
  typedef audioAnalyzer analyzer;

  audioStream();
  virtual ~audioStream();

  void loadFile(const std::string &fileURI);
  void start();
  void stop();
  const bool isPlaying() const;
  void renderImGui() const;
  const analyzer::audioAnalyzedFrame& getLatestFrame() const;
private:
  static constexpr unsigned FRAMES_PER_BUFFER = 64;

  int getSample(const void *inputBuffer, void *outputBuffer,
      unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
      PaStreamCallbackFlags statusFlags);
  void analysis();

  AudioFile<double> file;
  std::unique_ptr<portaudio::MemFunCallbackStream<audioStream> > stream;

  unsigned long pos;
  std::thread analysisThread;
  std::array<
      boost::lockfree::spsc_queue<float,
          boost::lockfree::capacity<FRAMES_PER_BUFFER * 32> >, analyzer::CHANNELS> analysisQueue; // 32 is completely arbitrary
  analyzer analysisEngine;
};

#endif /* AUDIOSTREAM_H_ */
