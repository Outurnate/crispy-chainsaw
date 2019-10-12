#ifndef AUDIOANALYZER_HPP
#define AUDIOANALYZER_HPP

#include "audioSystem.hpp"

class audioAnalyzer
{
public:
  audioAnalyzer();
  virtual ~audioAnalyzer();

  void analyze(const audioSourceFrame& sample);
  const audioAnalyzedFrame& getData() const;
private:
  audioAnalyzedFrame currentFrame, previousFrame;
};

#endif /* SRC_AUDIOANALYZER_H_ */
