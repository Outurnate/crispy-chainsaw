/*
 * audioAnalyzer.h
 *
 *  Created on: Oct 7, 2019
 *      Author: joseph
 */

#ifndef SRC_AUDIOANALYZER_H_
#define SRC_AUDIOANALYZER_H_

#include <array>

// 20-60 Hz sub bass
// 60-250 Hz bass
// 250-500 Hz low midrange
// 500-2000 Hz midrange
// 2000-4000 Hz upper midrange
// 4000-6000 Hz presence
// 6000-20000 Hz brilliance
// https://dlbeer.co.nz/articles/fftvis.html
// https://stackoverflow.com/a/20584591
enum class spectrumRange : unsigned
{
  subBass = 0, // 20-60 Hz
  bass = 1, // 60-250 Hz
  lowMidrange = 2, // 250-500 Hz
  midrange = 3, // 500-2000 Hz
  upperMidrange = 4, // 2000-4000 Hz
  presence = 5, // 4000-6000 Hz
  brilliance = 6  // 6000-20000 Hz
};

struct audioPoint
{
  float magnitude;
  float balance;

  audioPoint operator+(const audioPoint& rhs) const { return { this->magnitude + rhs.magnitude, this->balance + rhs.balance }; }
  audioPoint operator-(const audioPoint& rhs) const { return { this->magnitude - rhs.magnitude, this->balance - rhs.balance }; }
};

template<unsigned FFT_BINS, unsigned SAMPLE_RATE>
struct binLabels
{
  double labels[FFT_BINS];
  constexpr binLabels() : labels()
  {
    for (auto i = 0; i != FFT_BINS; ++i)
      labels[i] = i * SAMPLE_RATE / FFT_BINS;
  }
};

template<unsigned FFT_BINS, unsigned SAMPLE_RATE>
constexpr size_t getIndex(const binLabels<FFT_BINS, SAMPLE_RATE> labels, double upperBound)
{
  size_t maxIndex = 0;
  for (size_t i = 0; i != FFT_BINS; ++i)
    if (labels.labels[i] < upperBound)
      maxIndex = i;
  return maxIndex;
}

class audioAnalyzer
{
public:
  static constexpr unsigned CHANNELS = 2;
  static constexpr unsigned SAMPLE_RATE = 44100;
  static constexpr unsigned MINIMUM_FREQUENCY = 20;
  static constexpr unsigned WINDOW_SIZE = SAMPLE_RATE / MINIMUM_FREQUENCY;
  static constexpr unsigned FFT_BINS = (WINDOW_SIZE / 2) + 1;
  static constexpr unsigned MAXIMUM_FREQUENCY = (FFT_BINS - 1) * SAMPLE_RATE / FFT_BINS;

  typedef std::array<audioPoint, FFT_BINS> fftSpectrumData;
  typedef std::array<std::array<float, WINDOW_SIZE>, CHANNELS> audioSourceFrame;

  struct audioAnalyzedFrame
  {
    fftSpectrumData spectrum;
    bool hasBeat;
    double tempo;
  };

  audioAnalyzer();
  virtual ~audioAnalyzer();

  void analyze(const audioSourceFrame& sample);
  const audioAnalyzedFrame& getData() const;
private:
  audioAnalyzedFrame lastFrame;

  static constexpr binLabels<FFT_BINS, SAMPLE_RATE> labels = binLabels<FFT_BINS, SAMPLE_RATE>();

  static constexpr size_t subBassIndex = getIndex(labels, 60);
  static constexpr size_t bassIndex = getIndex(labels, 250);
  static constexpr size_t lowMidrangeIndex = getIndex(labels, 500);
  static constexpr size_t midrangeIndex = getIndex(labels, 2000);
  static constexpr size_t upperMidrangeIndex = getIndex(labels, 4000);
  static constexpr size_t presenceIndex = getIndex(labels, 6000);

  static inline constexpr size_t getStartIndex(spectrumRange range)
  {
    size_t val = 0;
    switch (range)
    {
    case spectrumRange::subBass:
      val = 0;
      break;
    case spectrumRange::bass:
      val = subBassIndex;
      break;
    case spectrumRange::lowMidrange:
      val = bassIndex;
      break;
    case spectrumRange::midrange:
      val = lowMidrangeIndex;
      break;
    case spectrumRange::upperMidrange:
      val = midrangeIndex;
      break;
    case spectrumRange::presence:
      val = upperMidrangeIndex;
      break;
    case spectrumRange::brilliance:
      val = presenceIndex;
      break;
    }
    return val;
  }

  static inline constexpr size_t getEndIndex(spectrumRange range)
  {
    size_t val = 0;
    switch (range)
    {
    case spectrumRange::subBass:
      val = subBassIndex;
      break;
    case spectrumRange::bass:
      val = bassIndex;
      break;
    case spectrumRange::lowMidrange:
      val = lowMidrangeIndex;
      break;
    case spectrumRange::midrange:
      val = midrangeIndex;
      break;
    case spectrumRange::upperMidrange:
      val = upperMidrangeIndex;
      break;
    case spectrumRange::presence:
      val = presenceIndex;
      break;
    case spectrumRange::brilliance:
      val = FFT_BINS;
      break;
    }
    return val;
  }

public:
  static inline constexpr size_t rangedSize(spectrumRange range)
  {
    return getEndIndex(range) - getStartIndex(range);
  }

  static inline const fftSpectrumData::const_iterator rangedBegin(const fftSpectrumData& array, spectrumRange range)
  {
    return array.cbegin() + getStartIndex(range);
  }

  static inline const fftSpectrumData::const_iterator rangedEnd(const fftSpectrumData& array, spectrumRange range)
  {
    return array.cbegin() + getEndIndex(range);
  }
};

#endif /* SRC_AUDIOANALYZER_H_ */
