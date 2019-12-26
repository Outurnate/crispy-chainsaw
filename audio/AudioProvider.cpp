#include "AudioProvider.hpp"

#include <functional>
#include <vector>
#include <spdlog/spdlog.h>

using namespace std::placeholders;

AudioProvider::AudioProvider(SoundIO::System& system)
  : device(system),
    stream(device, std::bind(&AudioProvider::readCallback, this, _1, _2, _3), SoundIoFormatFloat32NE, AudioSystem::SAMPLE_RATE)
{
  stream.start();
}

AudioProviderFrame AudioProvider::provide(int frames)
{
  AudioProviderFrame result;
  result.left  = std::vector<float>(frames);
  result.right = std::vector<float>(frames);
  for (int i = 0; i < frames; ++i)
  {
    StereoSample sample;
    if (!readSamples.pop(sample))
    {
      sample.left = 0.0f;
      sample.right = 0.0f;
      spdlog::warn("Provider underrun; emitting silence");
    }
    result.left.at(i)  = sample.left;
    result.right.at(i) = sample.right;
  }
  return result;
}

void AudioProvider::readCallback(SoundIO::InStream& stream, int minFrames, int maxFrames)
{
  (void)minFrames;

  int framesToRead = maxFrames;

  while (true)
  {
    int framesRead = framesToRead;
    bool success = stream.beginRead(framesRead);

    if (!framesRead)
      break;

    std::vector<StereoSample> tempSamples(framesRead);

    // fill tempSamples
    if (!success)
    {
      for (unsigned i = 0; i < framesRead; ++i)
      {
        tempSamples[i].left  = 0;
        tempSamples[i].right = 0;
        spdlog::warn("libsoundio underrun; putting silence into buffer");
      }
    }
    else
    {
      auto left  = stream.channel<float>(CHANNEL_LEFT);
      auto right = stream.channel<float>(CHANNEL_RIGHT);
      for (unsigned i = 0; i < framesRead; ++i)
      {
        tempSamples[i].left  = left[i];
        tempSamples[i].right = right[i];
      }
    }

    // push tempSamples
    if (int framesPushed = readSamples.push(tempSamples.data(), tempSamples.size());
        framesPushed != framesRead)
    {
      spdlog::warn("Provider overrun; dropping read frame");
    }

    stream.endRead();
    framesToRead -= framesRead;
    if (framesToRead <= 0)
      break;
  }
}
