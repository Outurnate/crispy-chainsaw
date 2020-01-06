#ifndef AUDIOPROVIDER_HPP
#define AUDIOPROVIDER_HPP

#include "AudioSystem.hpp"
#include "SoundIO.hpp"

#include <boost/lockfree/spsc_queue.hpp>

class AudioProvider
{
public:
  AudioProvider(SoundIO::System& system);

  void start();
  AudioProviderFrame provide(int frames);
private:
  void readCallback(SoundIO::InStream& stream, int minFrames, int maxFrames);

  SoundIO::InputDevice device;
  SoundIO::InStream stream;
  boost::lockfree::spsc_queue<StereoSample, boost::lockfree::capacity<AudioSystem::SAMPLE_RATE * 5> > readSamples; // 5 second buffer
};

#endif
