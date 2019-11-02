#include "audioOutput.hpp"

#include <range/v3/algorithm/copy.hpp>

using namespace std::placeholders;

audioOutput::audioOutput(soundio::system& system, provideAudioCallback providerCallback, playedAudioCallback playedCallback)
  : providerCallback(providerCallback),
    playedCallback(playedCallback),
    device(system),
    stream(device, "death by cold fries", std::bind(&audioOutput::writeCallback, this, _1, _2, _3), SoundIoFormatFloat32NE, audioSystem::SAMPLE_RATE)
{
}

audioOutput::~audioOutput()
{
}

void audioOutput::start()
{
  stream.start();
}

void audioOutput::writeCallback(soundio::outStream& stream, int minFrames, int maxFrames)
{
  (void)minFrames;
  int framesLeft = maxFrames;

  while (framesLeft > 0)
  {
    int frameCount = framesLeft;
    stream.beginWrite(frameCount);

    if (!frameCount)
      break;

    audioProviderFrame providerFrame = providerCallback(frameCount);
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
