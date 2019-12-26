#include "AudioOutput.hpp"

#include <range/v3/algorithm/copy.hpp>

using namespace std::placeholders;

AudioOutput::AudioOutput(SoundIO::System& system, ProvideAudioCallback providerCallback, PlayedAudioCallback playedCallback)
  : providerCallback(providerCallback),
    playedCallback(playedCallback),
    device(system),
    stream(device, "death by cold fries", std::bind(&AudioOutput::writeCallback, this, _1, _2, _3), SoundIoFormatFloat32NE, AudioSystem::SAMPLE_RATE)
{
}

void AudioOutput::start()
{
  stream.start();
}

void AudioOutput::writeCallback(SoundIO::OutStream& stream, int minFrames, int maxFrames)
{
  (void)minFrames;
  int framesLeft = maxFrames;

  while (framesLeft > 0)
  {
    int frameCount = framesLeft;
    stream.beginWrite(frameCount);

    if (!frameCount)
      break;

    AudioProviderFrame providerFrame = providerCallback(frameCount);
    auto leftStreamFrame = stream.channel<float>(CHANNEL_LEFT);
    auto rightStreamFrame = stream.channel<float>(CHANNEL_RIGHT);

    assert(providerFrame.left.size()  == leftStreamFrame.size());
    assert(providerFrame.right.size() == rightStreamFrame.size());

    ranges::v3::copy(providerFrame.left,  leftStreamFrame.begin());
    ranges::v3::copy(providerFrame.right, rightStreamFrame.begin());

    stream.endWrite();
    playedCallback(providerFrame, stream.getLatency());

    framesLeft -= frameCount;
  }
}
