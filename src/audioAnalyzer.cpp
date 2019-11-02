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

float adjustFFTResult(Complex bin, float currentFrequency, float previousValue, float alpha, float gamma, float scale, float exponent)
{
  float magnitude = sqrt(pow((bin.real()), 2) + pow((bin.imag()), 2));
  float gammaCoefficient = calculateGamma(currentFrequency, audioSystem::MAXIMUM_FREQUENCY, gamma);
  float previousMagnitude = previousValue / gammaCoefficient;
  return clamp(float(scale * pow(gammaCoefficient * lerp(previousMagnitude, magnitude, alpha), exponent)), 0.0f, 1.0f);
}

std::array<float, audioSystem::FFT_BINS> audioAnalyzer::analyzeChannel(const std::array<float, audioSystem::WINDOW_SIZE>& channelData, const std::array<float, audioSystem::FFT_BINS>& lastFrame, float alpha, float gamma, float scale, float exponent)
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
    result[i] = adjustFFTResult(
        transformedSample[i],
        audioSystem::labels.labels[i],
        lastFrame[i],
        alpha,
        gamma,
        scale,
        exponent);

  return result;
}

const fftSpectrumData& audioAnalyzer::analyze(const audioSourceFrame& sample, float alpha, float gamma, float scale, float exponent)
{
  stereoPair<std::array<float, audioSystem::FFT_BINS> > analyzedSamples
  {
    .left  = analyzeChannel(sample.left,  lastFrame.left,  alpha, gamma, scale, exponent),
    .right = analyzeChannel(sample.right, lastFrame.right, alpha, gamma, scale, exponent),
  };

  lastFrame = analyzedSamples;

  fftSpectrumData audioPoints;
  for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
  {
    audioPoints[i].magnitude = (analyzedSamples.left[i] + analyzedSamples.right[i]) / 2;
    audioPoints[i].balance = clamp(analyzedSamples.left[i] - analyzedSamples.right[i], -2.0f, 2.0f);
  }

  return currentData = audioPoints;
}
