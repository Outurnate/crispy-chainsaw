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
  result.left  = ranges::v3::span<float>(&file.samples[CHANNEL_LEFT ][position], frames);
  result.right = ranges::v3::span<float>(&file.samples[CHANNEL_RIGHT][position], frames);
  position += frames;
  return result;
}
