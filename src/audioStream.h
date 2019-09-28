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

static constexpr unsigned FRAMES_PER_BUFFER = 64;
static constexpr unsigned CHANNELS = 2;
static constexpr unsigned SAMPLE_RATE = 44100;
static constexpr unsigned MINIMUM_FREQUENCY = 20;
static constexpr unsigned WINDOW_SIZE = SAMPLE_RATE / MINIMUM_FREQUENCY;
static constexpr unsigned FFT_BINS = (WINDOW_SIZE / 2) + 1;
static constexpr unsigned MAXIMUM_FREQUENCY = (FFT_BINS - 1) * SAMPLE_RATE / FFT_BINS;

struct binLabels
{
  double labels[FFT_BINS];
  constexpr binLabels() : labels()
  {
    for (auto i = 0; i != FFT_BINS; ++i)
      labels[i] = i * SAMPLE_RATE / FFT_BINS;
  }
};

constexpr size_t getIndex(const binLabels labels, double upperBound)
{
  size_t maxIndex = 0;
  for (size_t i = 0; i != FFT_BINS; ++i)
    if (labels.labels[i] < upperBound)
      maxIndex = i;
  return maxIndex;
}

static constexpr binLabels labels;
static constexpr size_t subBassIndex = getIndex(labels, 60);
static constexpr size_t bassIndex = getIndex(labels, 250);
static constexpr size_t lowMidrangeIndex = getIndex(labels, 500);
static constexpr size_t midrangeIndex = getIndex(labels, 2000);
static constexpr size_t upperMidrangeIndex = getIndex(labels, 4000);
static constexpr size_t presenceIndex = getIndex(labels, 6000);

class audioStream : private portaudio::AutoSystem
{
public:
  typedef std::array<std::vector<float>, CHANNELS> fftResult;

  class analysisResult
  {
    friend class audioStream;
  public:
    analysisResult();
    virtual ~analysisResult();

    boost::circular_buffer<fftResult> analyzedFFT;
    fftResult movingAverageFFT;
    std::unordered_map<spectrumRange, fftResult> rangedMovingAverageFFT,
        rangedLatestResult;
  private:
    static const fftResult emptyResult;

    void update();
  };

  audioStream();
  virtual ~audioStream();

  void loadFile(const std::string &fileURI);
  void start();
  void stop();
  const bool isPlaying() const;
  const analysisResult& getResult() const;

  void renderImGui() const;
private:
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
          boost::lockfree::capacity<FRAMES_PER_BUFFER * 32> >, CHANNELS> analysisQueue; // 32 is completely arbitrary
  analysisResult result;
};

#endif /* AUDIOSTREAM_H_ */
