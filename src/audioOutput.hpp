#ifndef AUDIOOUTPUT_HPP
#define AUDIOOUTPUT_HPP

#include "soundIO.hpp"
#include "audioSystem.hpp"

#include <functional>
#include <array>
#include <chrono>

class audioOutput
{
public:
  // frames to return
  typedef std::function<audioProviderFrame(int)> provideAudioCallback;
  // frame, latency
  typedef std::function<void(const audioProviderFrame&,std::chrono::duration<double>)> playedAudioCallback;

  audioOutput(soundio::system& system, provideAudioCallback providerCallback, playedAudioCallback playedCallback);
  virtual ~audioOutput();

  void start();
private:
  void writeCallback(soundio::outStream& stream, int minFrames, int maxFrames);

  provideAudioCallback providerCallback;
  playedAudioCallback playedCallback;
  soundio::outputDevice device;
  soundio::outStream stream;
};

#endif
