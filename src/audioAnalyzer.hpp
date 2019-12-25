#ifndef AUDIOANALYZER_HPP
#define AUDIOANALYZER_HPP

#include "audioSystem.hpp"

class AudioAnalyzer
{
public:
  AudioAnalyzer();

  const FFTSpectrumData& analyze(const AudioSourceFrame& sample, float alpha, float gamma, float scale, float exponent);
private:
  std::array<float, AudioSystem::FFT_BINS> analyzeChannel(const std::array<float, AudioSystem::WINDOW_SIZE>& channelData, const std::array<float, AudioSystem::FFT_BINS>& lastFrame, float alpha, float gamma, float scale, float exponent);
  StereoPair<std::array<float, AudioSystem::FFT_BINS> > lastFrame;
  FFTSpectrumData currentData;
};

#endif
