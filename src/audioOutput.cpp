#include "audioOutput.hpp"

#include <range/v3/algorithm/copy.hpp>

using namespace std::placeholders;

audioOutput::audioOutput(soundio::system& system, provideAudioCallback providerCallback)
  : providerCallback(providerCallback),
    device(system),
    stream(device, "death by cold fries", std::bind(&audioOutput::writeCallback, this, _1, _2, _3), SoundIoFormatFloat32NE, audioSystem::SAMPLE_RATE)
{
  stream.start();
}

audioOutput::~audioOutput()
{
}

void audioOutput::writeCallback(soundio::outStream& stream, int minFrames, int maxFrames)
{
  int framesLeft = maxFrames;

  while (framesLeft > 0)
  {
    int frameCount = framesLeft;
    stream.beginWrite(frameCount);

    if (!frameCount)
      break;

    audioProviderFrame providerFrame = providerCallback(frameCount);
    for (int channel = 0; channel < stream.getChannels(); ++channel)
    {
      auto channelSpan = stream.channel<float>(channel);
      assert(channelSpan.size() == providerFrame[channel].size());
      ranges::v3::copy(providerFrame[channel], channelSpan.begin());
    }

    stream.endWrite();

    framesLeft -= frameCount;
  }
}
