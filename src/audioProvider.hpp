#ifndef AUDIOPROVIDER_HPP
#define AUDIOPROVIDER_HPP

#include "audioSystem.hpp"
#include "soundIO.hpp"

#include <boost/lockfree/spsc_queue.hpp>

class audioProvider
{
public:
  audioProvider(soundio::system& system);
  virtual ~audioProvider();

  audioProviderFrame provide(int frames);
private:
  void readCallback(soundio::inStream& stream, int minFrames, int maxFrames);

  soundio::inputDevice device;
  soundio::inStream stream;
  boost::lockfree::spsc_queue<stereoSample, boost::lockfree::capacity<audioSystem::SAMPLE_RATE * 5> > readSamples; // 5 second buffer
};

#endif
