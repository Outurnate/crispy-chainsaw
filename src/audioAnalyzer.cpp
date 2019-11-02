#include "audioAnalyzer.hpp"
#include "audioEngine.hpp"

#include <fftwpp/Array.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#include <fftwpp/fftw++.h>
#pragma clang diagnostic pop

#include "math.hpp"

// https://dlbeer.co.nz/articles/fftvis.html
// https://stackoverflow.com/a/20584591

audioAnalyzer::audioAnalyzer()
{
  fftwpp::fftw::maxthreads = get_max_threads();
}

audioAnalyzer::~audioAnalyzer()
{
}

inline float calculateGamma(float currentFrequency, float maxFrequency, float gamma)
{
  return pow((currentFrequency / maxFrequency), (1 / gamma));
}

std::array<float, audioSystem::FFT_BINS> audioAnalyzer::analyzeChannel(const std::array<float, audioSystem::WINDOW_SIZE>& channelData)
{
  std::array<float, audioSystem::FFT_BINS> result;

  size_t alignment = sizeof(Complex);
  Array::array1<double> windowedSample(audioSystem::WINDOW_SIZE, alignment);

  for (unsigned i = 0; i < audioSystem::WINDOW_SIZE; ++i)
  {
    // apply window function to all samples to reduce spectral leakage
    double w = 0.5 * (1 - cos(2 * M_PI * i / (audioSystem::WINDOW_SIZE - 1)));
    windowedSample[i] = w * channelData[i];
  }

  Array::array1<Complex> transformedSample(audioSystem::FFT_BINS, alignment);
  fftwpp::rcfft1d forward(audioSystem::WINDOW_SIZE, windowedSample, transformedSample);
  forward.fft(windowedSample, transformedSample);

  for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
  {
    float d = sqrt(pow((transformedSample[i].real()), 2) + pow((transformedSample[i].imag()), 2));
    float currentFrequency = audioSystem::labels.labels[i];
    float gammaCoefficient = calculateGamma(currentFrequency, audioSystem::MAXIMUM_FREQUENCY, currentParams.gamma);
    result[i] = clamp(currentParams.scale * gammaCoefficient * lerp(currentFrame.spectrum[i].magnitude / gammaCoefficient, d, currentParams.alpha), 0.0f, 1.0f); // TODO alpha calc bleeds between channels
  }

  return result;
}

void audioAnalyzer::analyze(const audioSourceFrame& sample)
{
  stereoPair<std::array<float, audioSystem::FFT_BINS> > analyzedSamples
  {
    .left  = analyzeChannel(sample.left),
    .right = analyzeChannel(sample.right),
  };

  fftSpectrumData audioPoints;
  for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
  {
    audioPoints[i].magnitude = (analyzedSamples.left[i] + analyzedSamples.right[i]) / 2;
    audioPoints[i].balance = clamp(analyzedSamples.left[i] - analyzedSamples.right[i], -2.0f, 2.0f);
  }

  currentFrame.spectrum = audioPoints;
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
