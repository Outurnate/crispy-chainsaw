#include "audioProvider.hpp"

audioProvider::audioProvider()
  : position(0)
{
  file.load("/home/joseph/eclipse/audio_bak/Debug/test.wav");
}

audioProvider::~audioProvider() {}

audioProviderFrame audioProvider::provide(int frames)
{
  audioProviderFrame result;
  for (unsigned channel = 0; channel < audioSystem::CHANNELS; ++channel)
    result[channel] = ranges::v3::span<float>(&file.samples[channel][position], frames);
  position += frames;
  return result;
}
