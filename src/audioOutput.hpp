#ifndef AUDIOOUTPUT_HPP
#define AUDIOOUTPUT_HPP

#include "soundIO.hpp"
#include "audioSystem.hpp"

#include <functional>
#include <array>
#include <chrono>

class AudioOutput
{
public:
  // frames to return
  typedef std::function<AudioProviderFrame(int)> ProvideAudioCallback;
  // frame, latency
  typedef std::function<void(const AudioProviderFrame&,std::chrono::duration<double>)> PlayedAudioCallback;

  AudioOutput(SoundIO::System& system, ProvideAudioCallback providerCallback, PlayedAudioCallback playedCallback);

  void start();
private:
  void writeCallback(SoundIO::OutStream& stream, int minFrames, int maxFrames);

  ProvideAudioCallback providerCallback;
  PlayedAudioCallback playedCallback;
  SoundIO::OutputDevice device;
  SoundIO::OutStream stream;
};

#endif
