#ifndef AUDIOSYSTEM_HPP
#define AUDIOSYSTEM_HPP

#include <array>
#include <range/v3/span.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/numeric/accumulate.hpp>

#define CHANNEL_LEFT  0
#define CHANNEL_RIGHT 1

enum class SpectrumRange : unsigned
{
  SubBass = 0, // 20-60 Hz
  Bass = 1, // 60-250 Hz
  LowMidrange = 2, // 250-500 Hz
  Midrange = 3, // 500-2000 Hz
  UpperMidrange = 4, // 2000-4000 Hz
  Presence = 5, // 4000-6000 Hz
  Brilliance = 6  // 6000-20000 Hz
};

enum class Channel : unsigned
{
  Left = CHANNEL_LEFT,
  Right = CHANNEL_RIGHT,
  Average = 1234
};

struct AudioPoint
{
  float magnitude;
  float balance;

  float getChannel(Channel which) const
  {
    switch (which)
    {
    case Channel::Left:
      return ((2 * magnitude) + balance) / 2;
    case Channel::Right:
      return ((2 * magnitude) - balance) / 2;
    default:
    case Channel::Average:
      return magnitude;
    }
  }

  AudioPoint operator+(const AudioPoint& rhs) const { return { this->magnitude + rhs.magnitude, this->balance + rhs.balance }; }
  AudioPoint operator-(const AudioPoint& rhs) const { return { this->magnitude - rhs.magnitude, this->balance - rhs.balance }; }

  AudioPoint() : magnitude(0.0f), balance(0.0f) {}
  AudioPoint(float magnitude, float balance) : magnitude(magnitude), balance(balance) {}
};

template<typename T>
struct StereoPair
{
  T left;
  T right;
};

template<unsigned FFT_BINS, unsigned SAMPLE_RATE>
struct BinLabels
{
  double labels[FFT_BINS];
  constexpr BinLabels() : labels()
  {
    for (auto i = 0; i != FFT_BINS; ++i)
      labels[i] = i * SAMPLE_RATE / FFT_BINS;
  }
};

template<unsigned FFT_BINS, unsigned SAMPLE_RATE>
constexpr size_t getIndex(const BinLabels<FFT_BINS, SAMPLE_RATE> labels, double upperBound)
{
  size_t maxIndex = 0;
  for (size_t i = 0; i != FFT_BINS; ++i)
    if (labels.labels[i] < upperBound)
      maxIndex = i;
  return maxIndex;
}

class AudioSystem
{
public:
  static constexpr unsigned SAMPLE_RATE = 44100;
  static constexpr unsigned MINIMUM_FREQUENCY = 20;
  static constexpr unsigned WINDOW_SIZE = SAMPLE_RATE / MINIMUM_FREQUENCY;
  static constexpr unsigned FFT_BINS = (WINDOW_SIZE / 2) + 1;
  static constexpr unsigned MAXIMUM_FREQUENCY = (FFT_BINS - 1) * SAMPLE_RATE / FFT_BINS;

  static constexpr BinLabels<AudioSystem::FFT_BINS, AudioSystem::SAMPLE_RATE> labels = BinLabels<AudioSystem::FFT_BINS, AudioSystem::SAMPLE_RATE>();

  static constexpr size_t subBassIndex = getIndex(labels, 60);
  static constexpr size_t bassIndex = getIndex(labels, 250);
  static constexpr size_t lowMidrangeIndex = getIndex(labels, 500);
  static constexpr size_t midrangeIndex = getIndex(labels, 2000);
  static constexpr size_t upperMidrangeIndex = getIndex(labels, 4000);
  static constexpr size_t presenceIndex = getIndex(labels, 6000);
};

typedef std::array<AudioPoint, AudioSystem::FFT_BINS> FFTSpectrumData;
typedef StereoPair<std::array<float, AudioSystem::WINDOW_SIZE> > AudioSourceFrame;
typedef StereoPair<std::vector<float> > AudioProviderFrame;
typedef StereoPair<float> StereoSample;

static inline constexpr size_t getStartIndex(SpectrumRange range)
{
  size_t val = 0;
  switch (range)
  {
  case SpectrumRange::SubBass:
    val = 0;
    break;
  case SpectrumRange::Bass:
    val = AudioSystem::subBassIndex;
    break;
  case SpectrumRange::LowMidrange:
    val = AudioSystem::bassIndex;
    break;
  case SpectrumRange::Midrange:
    val = AudioSystem::lowMidrangeIndex;
    break;
  case SpectrumRange::UpperMidrange:
    val = AudioSystem::midrangeIndex;
    break;
  case SpectrumRange::Presence:
    val = AudioSystem::upperMidrangeIndex;
    break;
  case SpectrumRange::Brilliance:
    val = AudioSystem::presenceIndex;
    break;
  }
  return val;
}

static inline constexpr size_t getEndIndex(SpectrumRange range)
{
  size_t val = 0;
  switch (range)
  {
  case SpectrumRange::SubBass:
    val = AudioSystem::subBassIndex;
    break;
  case SpectrumRange::Bass:
    val = AudioSystem::bassIndex;
    break;
  case SpectrumRange::LowMidrange:
    val = AudioSystem::lowMidrangeIndex;
    break;
  case SpectrumRange::Midrange:
    val = AudioSystem::midrangeIndex;
    break;
  case SpectrumRange::UpperMidrange:
    val = AudioSystem::upperMidrangeIndex;
    break;
  case SpectrumRange::Presence:
    val = AudioSystem::presenceIndex;
    break;
  case SpectrumRange::Brilliance:
    val = AudioSystem::FFT_BINS;
    break;
  }
  return val;
}

static inline constexpr size_t spectrumSize(SpectrumRange range)
{
  return getEndIndex(range) - getStartIndex(range);
}

static inline constexpr size_t spectrumSize(SpectrumRange rangeBegin, SpectrumRange rangeEnd)
{
  return getEndIndex(rangeEnd) - getStartIndex(rangeBegin);
}

static inline auto spectrumView(const FFTSpectrumData& array, SpectrumRange range)
{
  return array | ranges::views::slice(getStartIndex(range), getEndIndex(range));
}

static inline auto spectrumView(const FFTSpectrumData& array, SpectrumRange rangeBegin, SpectrumRange rangeEnd)
{
  return array | ranges::views::slice(getStartIndex(rangeBegin), getEndIndex(rangeEnd));
}

static inline float spectrumAverage(const FFTSpectrumData& array, SpectrumRange range, Channel sampleChannel = Channel::Average)
{
  float sum = ranges::accumulate(
      spectrumView(array, range),
      0.0f,
      [sampleChannel](const float a, const AudioPoint& b){ return a + b.getChannel(sampleChannel); });
  float count = spectrumSize(range);

  return sum / count;
}

static inline float spectrumAverage(const FFTSpectrumData& array, SpectrumRange rangeBegin, SpectrumRange rangeEnd, Channel sampleChannel = Channel::Average)
{
  float sum = ranges::accumulate(
      spectrumView(array, rangeBegin, rangeEnd),
      0.0f,
      [sampleChannel](const float a, const AudioPoint& b){ return a + b.getChannel(sampleChannel); });
  float count = spectrumSize(rangeBegin, rangeEnd);

  return sum / count;
}

#endif
