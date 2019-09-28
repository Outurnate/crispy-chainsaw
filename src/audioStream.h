/*
 * audioStream.h
 *
 *  Created on: Sep 21, 2019
 *      Author: joseph
 */

#ifndef AUDIOSTREAM_H_
#define AUDIOSTREAM_H_

#include <memory>
#include <thread>
#include <array>
#include <vector>
#include <unordered_map>
#include <boost/circular_buffer.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <portaudiocpp/PortAudioCpp.hxx>
#include <AudioFile.h>
#include <fftwpp/Array.h>

#define FRAMES_PER_BUFFER 64
#define CHANNELS 2
#define SAMPLE_RATE 44100
#define MINIMUM_FREQUENCY 20
#define WINDOW_SIZE (SAMPLE_RATE / MINIMUM_FREQUENCY)
#define FFT_BINS ((WINDOW_SIZE / 2) + 1)
#define MAXIMUM_FREQUENCY ((FFT_BINS - 1) * SAMPLE_RATE / FFT_BINS)

class audioStream
{
public:
  typedef std::array<std::vector<float>, CHANNELS> fftResult;

  class analysisResult
  {
    friend class audioStream;
  public:
    enum spectrumRange : unsigned
    {
      subBass = 0, // 20-60 Hz
      bass = 1, // 60-250 Hz
      lowMidrange = 2, // 250-500 Hz
      midrange = 3, // 500-2000 Hz
      upperMidrange = 4, // 2000-4000 Hz
      presence = 5, // 4000-6000 Hz
      brilliance = 6  // 6000-20000 Hz
    };

    analysisResult();

    boost::circular_buffer<fftResult> analyzedFFT;
    fftResult movingAverageFFT;
    std::unordered_map<spectrumRange, fftResult> rangedMovingAverageFFT,
        rangedLatestResult;
  private:
    static const fftResult emptyResult;

    void update(const std::vector<double> &fftFrequencies);
  };

  audioStream();
  virtual ~audioStream();

  void loadFile(const std::string &fileURI);
  void start();
  void stop();
  const bool isPlaying() const;

  void renderImGui() const;
private:
  int getSample(const void *inputBuffer, void *outputBuffer,
      unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
      PaStreamCallbackFlags statusFlags);
  void analysis();

  portaudio::AutoSystem sys;
  const portaudio::Device &device;
  AudioFile<double> file;
  std::unique_ptr<portaudio::MemFunCallbackStream<audioStream> > stream;
  unsigned long pos;
  std::thread analysisThread;
  std::array<
      boost::lockfree::spsc_queue<float,
          boost::lockfree::capacity<FRAMES_PER_BUFFER * 32> >, CHANNELS> analysisQueue; // 32 is completely arbitrary
  std::vector<double> fftFrequencies;
  analysisResult result;
};

#endif /* AUDIOSTREAM_H_ */
