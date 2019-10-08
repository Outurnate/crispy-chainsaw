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

audioAnalyzer::audioAnalyzer()
{
}

audioAnalyzer::~audioAnalyzer()
{
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
      double d = sqrt(
          pow(transformedSample[i].real(), 2)
              + pow(transformedSample[i].imag(), 2));
      analyzedSample[channel][i] = d;
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

  lastFrame.spectrum = audioPoints;
}

const audioAnalyzer::audioAnalyzedFrame& audioAnalyzer::getData() const
{
  return lastFrame;
}
