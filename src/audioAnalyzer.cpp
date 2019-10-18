#include "audioAnalyzer.hpp"
#include "audioEngine.hpp"

#include <fftwpp/Array.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#include <fftwpp/fftw++.h>
#pragma clang diagnostic pop

// https://dlbeer.co.nz/articles/fftvis.html
// https://stackoverflow.com/a/20584591

audioAnalyzer::audioAnalyzer()
  : beatTracker(audioSystem::WINDOW_SIZE / 2, audioSystem::WINDOW_SIZE)
{
  fftwpp::fftw::maxthreads = get_max_threads();
}

audioAnalyzer::~audioAnalyzer()
{
}

template<typename T>
inline T lerp(T a, T b, T alpha)
{
  return alpha * a + ((1 - alpha) * b);
}

template<typename T>
inline T clamp(T a, T minV, T maxV)
{
  return std::min(std::max(minV, a), maxV);
}

inline float calculateGamma(float currentFrequency, float maxFrequency, float gamma)
{
  return pow((currentFrequency / maxFrequency), (1 / gamma));
}

void audioAnalyzer::analyze(const audioSourceFrame& sample)
{
  std::array<std::array<float, audioSystem::FFT_BINS>, audioSystem::CHANNELS> analyzedSample;

  // analyze a sample
  for (unsigned channel = 0; channel < audioSystem::CHANNELS; ++channel)
  {
    size_t alignment = sizeof(Complex);
    Array::array1<double> windowedSample(audioSystem::WINDOW_SIZE, alignment);

    for (unsigned i = 0; i < audioSystem::WINDOW_SIZE; ++i)
    {
      // apply window function to all samples to reduce spectral leakage
      double w = 0.5 * (1 - cos(2 * M_PI * i / (audioSystem::WINDOW_SIZE - 1)));
      windowedSample[i] = w * sample[channel][i];
    }

    Array::array1<Complex> transformedSample(audioSystem::FFT_BINS, alignment);
    fftwpp::rcfft1d forward(audioSystem::WINDOW_SIZE, windowedSample, transformedSample);
    forward.fft(windowedSample, transformedSample);

    for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
    {
      float d = sqrt(pow((transformedSample[i].real()), 2) + pow((transformedSample[i].imag()), 2));
      float currentFrequency = audioSystem::labels.labels[i];
      float gammaCoefficient = calculateGamma(currentFrequency, audioSystem::MAXIMUM_FREQUENCY, currentParams.gamma);
      analyzedSample[channel][i] = clamp(currentParams.scale * gammaCoefficient * lerp(currentFrame.spectrum[i].magnitude / gammaCoefficient, d, currentParams.alpha), 0.0f, 1.0f); // TODO alpha calc bleeds between channels
    }
  }

  fftSpectrumData audioPoints;
  for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
  {
    float sum = 0.0f;
    for (unsigned channel = 0; channel < audioSystem::CHANNELS; ++channel)
      sum += analyzedSample[channel][i];
    audioPoints[i].magnitude = sum / audioSystem::CHANNELS;
    audioPoints[i].balance = clamp(audioSystem::CHANNELS == 2 ? (analyzedSample[0][i] - analyzedSample[1][i]) : 0.0f, -2.0f, 2.0f);
  }

  {
    audioSourceFrame temp = sample; //TODO modify BTrack so this isn't needed
    beatTracker.processAudioFrame(temp[0].data());
  }

  currentFrame.spectrum = audioPoints;
  currentFrame.tempo = beatTracker.getCurrentTempoEstimate();
}

const audioAnalyzedFrame& audioAnalyzer::getData() const
{
  return currentFrame;
}

const audioAnalyzer::params& audioAnalyzer::getParams() const
{
  return currentParams;
}

void audioAnalyzer::setParams(const audioAnalyzer::params& newParams)
{
  currentParams = newParams;
}
