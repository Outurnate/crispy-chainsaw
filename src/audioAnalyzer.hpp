#ifndef AUDIOANALYZER_HPP
#define AUDIOANALYZER_HPP

#include "audioSystem.hpp"

class audioAnalyzer
{
public:
  audioAnalyzer();
  virtual ~audioAnalyzer();

  void analyze(const audioSourceFrame& sample);
  const audioAnalyzedFrame& getData() const;
private:
  audioAnalyzedFrame currentFrame, previousFrame;

  static constexpr binLabels<audioSystem::FFT_BINS, audioSystem::SAMPLE_RATE> labels = binLabels<audioSystem::FFT_BINS, audioSystem::SAMPLE_RATE>();

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
      val = audioSystem::FFT_BINS;
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
