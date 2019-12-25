#include "audioAnalyzer.hpp"
#include "audioEngine.hpp"

#include <fftwpp/Array.h>
#include <fftwpp/fftw++.h>

#include "math.hpp"

// https://dlbeer.co.nz/articles/fftvis.html
// https://stackoverflow.com/a/20584591

AudioAnalyzer::AudioAnalyzer()
{
  fftwpp::fftw::maxthreads = get_max_threads();
}

inline float calculateGamma(float currentFrequency, float maxFrequency, float gamma)
{
  return pow((currentFrequency / maxFrequency), (1 / gamma));
}

float adjustFFTResult(Complex bin, float currentFrequency, float previousValue, float alpha, float gamma, float scale, float exponent)
{
  float magnitude = sqrt(pow((bin.real()), 2) + pow((bin.imag()), 2));
  float gammaCoefficient = calculateGamma(currentFrequency, AudioSystem::MAXIMUM_FREQUENCY, gamma);
  float previousMagnitude = previousValue / gammaCoefficient;
  return clamp(float(scale * pow(gammaCoefficient * lerp(previousMagnitude, magnitude, alpha), exponent)), 0.0f, 1.0f);
}

std::array<float, AudioSystem::FFT_BINS> AudioAnalyzer::analyzeChannel(const std::array<float, AudioSystem::WINDOW_SIZE>& channelData, const std::array<float, AudioSystem::FFT_BINS>& lastFrame, float alpha, float gamma, float scale, float exponent)
{
  std::array<float, AudioSystem::FFT_BINS> result;

  size_t alignment = sizeof(Complex);
  Array::array1<double> windowedSample(AudioSystem::WINDOW_SIZE, alignment);

  for (unsigned i = 0; i < AudioSystem::WINDOW_SIZE; ++i)
  {
    // apply window function to all samples to reduce spectral leakage
    double w = 0.5 * (1 - cos(2 * M_PI * i / (AudioSystem::WINDOW_SIZE - 1)));
    windowedSample[i] = w * channelData[i];
  }

  Array::array1<Complex> transformedSample(AudioSystem::FFT_BINS, alignment);
  fftwpp::rcfft1d forward(AudioSystem::WINDOW_SIZE, windowedSample, transformedSample);
  forward.fft(windowedSample, transformedSample);

  for (unsigned i = 0; i < AudioSystem::FFT_BINS; ++i)
    result[i] = adjustFFTResult(
        transformedSample[i],
        AudioSystem::labels.labels[i],
        lastFrame[i],
        alpha,
        gamma,
        scale,
        exponent);

  return result;
}

const FFTSpectrumData& AudioAnalyzer::analyze(const AudioSourceFrame& sample, float alpha, float gamma, float scale, float exponent)
{
  StereoPair<std::array<float, AudioSystem::FFT_BINS> > analyzedSamples
  {
    .left  = analyzeChannel(sample.left,  lastFrame.left,  alpha, gamma, scale, exponent),
    .right = analyzeChannel(sample.right, lastFrame.right, alpha, gamma, scale, exponent),
  };

  lastFrame = analyzedSamples;

  FFTSpectrumData audioPoints;
  for (unsigned i = 0; i < AudioSystem::FFT_BINS; ++i)
  {
    audioPoints[i].magnitude = (analyzedSamples.left[i] + analyzedSamples.right[i]) / 2;
    audioPoints[i].balance = clamp(analyzedSamples.left[i] - analyzedSamples.right[i], -2.0f, 2.0f);
  }

  return currentData = audioPoints;
}
