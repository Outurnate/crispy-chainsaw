#ifndef AUDIOOUTPUT_HPP
#define AUDIOOUTPUT_HPP

#include "soundIO.hpp"
#include "audioSystem.hpp"

#include <functional>
#include <array>

class audioOutput
{
public:
  // channel, frames to return
  typedef std::function<audioProviderFrame(int)> provideAudioCallback;

  audioOutput(soundio::system& system, provideAudioCallback providerCallback);
  virtual ~audioOutput();
private:
  void writeCallback(soundio::outStream& stream, int minFrames, int maxFrames);

  provideAudioCallback providerCallback;
  soundio::outputDevice device;
  soundio::outStream stream;
};

#endif
