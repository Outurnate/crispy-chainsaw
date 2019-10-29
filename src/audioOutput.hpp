#ifndef AUDIOOUTPUT_HPP
#define AUDIOOUTPUT_HPP

#include "soundIO.hpp"

class audioOutput
{
public:
  audioOutput(soundio::system& system);
  virtual ~audioOutput();
private:
  void callback(soundio::outStream& stream, int minFrames, int maxFrames);

  soundio::outputDevice device;
  soundio::outStream stream;
};

#endif
