#ifndef AUDIOANALYZER_HPP
#define AUDIOANALYZER_HPP

#include "audioSystem.hpp"
#include <BTrack.h>

class audioAnalyzer
{
public:
  struct params
  {
    float alpha = 0.2f;
    float gamma = 2.0f;
    float scale = 1.0f / 10.0f;
  };

  audioAnalyzer();
  virtual ~audioAnalyzer();

  void analyze(const audioSourceFrame& sample);
  const audioAnalyzedFrame& getData() const;
  const params& getParams() const;
  void setParams(const params& newParams);
private:
  audioAnalyzedFrame currentFrame;
  BTrack beatTracker;
  params currentParams;
};

#endif
