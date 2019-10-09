/*
 * audioAnalyzer.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: joseph
 */

#include "audioAnalyzer.h"

#include <fftwpp/Array.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#include <fftwpp/fftw++.h>
#pragma clang diagnostic pop

#include <iostream>

// https://dlbeer.co.nz/articles/fftvis.html
// https://stackoverflow.com/a/20584591

audioAnalyzer::audioAnalyzer()
{
}

audioAnalyzer::~audioAnalyzer()
{
}

template<typename T>
inline T lerp(T a, T b, T alpha)
{
  return alpha * a + ((1 - alpha) * b);
}

inline float calculateGamma(float currentFrequency, float maxFrequency, float gamma)
{
  return pow((currentFrequency / maxFrequency), (1 / gamma));
}

void audioAnalyzer::analyze(const audioSourceFrame& sample)
{
  fftwpp::fftw::maxthreads = get_max_threads();
  std::array<std::array<float, FFT_BINS>, CHANNELS> analyzedSample;

  // analyze a sample
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
  {
    size_t alignment = sizeof(Complex);
    Array::array1<double> windowedSample(WINDOW_SIZE, alignment);

    for (unsigned i = 0; i < WINDOW_SIZE; ++i)
    {
      // apply window function to all samples to reduce spectral leakage
      double w = 0.5 * (1 - cos(2 * M_PI * i / (WINDOW_SIZE - 1)));
      windowedSample[i] = w * sample[channel][i];
    }

    Array::array1<Complex> transformedSample(FFT_BINS, alignment);
    fftwpp::rcfft1d forward(WINDOW_SIZE, windowedSample, transformedSample);
    forward.fft(windowedSample, transformedSample);

    for (unsigned i = 0; i < FFT_BINS; ++i)
    {
      float d = sqrt(pow((transformedSample[i].real()), 2) + pow((transformedSample[i].imag()), 2));
      float currentFrequency = labels.labels[i];
      float gammaCoefficient = calculateGamma(currentFrequency, MAXIMUM_FREQUENCY, 2);
      analyzedSample[channel][i] = gammaCoefficient * lerp(previousFrame.spectrum[i].magnitude / gammaCoefficient, d, 0.20f);
    }
  }

  fftSpectrumData audioPoints;
  for (unsigned i = 0; i < FFT_BINS; ++i)
  {
    float sum = 0.0f;
    for (unsigned channel = 0; channel < CHANNELS; ++channel)
      sum += analyzedSample[channel][i];
    audioPoints[i].magnitude = sum / CHANNELS;
    audioPoints[i].balance = CHANNELS == 2 ? (analyzedSample[0][i] - analyzedSample[1][i]) : 0.0f;
  }

  previousFrame = currentFrame;
  currentFrame.spectrum = audioPoints;
}

const audioAnalyzer::audioAnalyzedFrame& audioAnalyzer::getData() const
{
  return currentFrame;
}
