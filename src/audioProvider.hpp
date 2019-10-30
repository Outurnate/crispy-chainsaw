#ifndef AUDIOPROVIDER_HPP
#define AUDIOPROVIDER_HPP

#include "audioSystem.hpp"
#include "AudioFile.h"

class audioProvider
{
public:
  audioProvider();
  virtual ~audioProvider();

  audioProviderFrame provide(int frames);
private:
  AudioFile<float> file;
  int position;
};

#endif
