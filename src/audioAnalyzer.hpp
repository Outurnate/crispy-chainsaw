#ifndef AUDIOANALYZER_HPP
#define AUDIOANALYZER_HPP

#include "audioSystem.hpp"

class audioAnalyzer
{
public:
  audioAnalyzer();
  virtual ~audioAnalyzer();

  const fftSpectrumData& analyze(const audioSourceFrame& sample, float alpha, float gamma, float scale, float exponent);
private:
  std::array<float, audioSystem::FFT_BINS> analyzeChannel(const std::array<float, audioSystem::WINDOW_SIZE>& channelData, const std::array<float, audioSystem::FFT_BINS>& lastFrame, float alpha, float gamma, float scale, float exponent);
  stereoPair<std::array<float, audioSystem::FFT_BINS> > lastFrame;
  fftSpectrumData currentData;
};

#endif
