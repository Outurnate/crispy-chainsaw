#include "audioProvider.hpp"

#include <functional>
#include <vector>

using namespace std::placeholders;

audioProvider::audioProvider(soundio::system& system)
  : device(system),
    stream(device, std::bind(&audioProvider::readCallback, this, _1, _2, _3))
{
  stream.start();
}

audioProvider::~audioProvider() {}

audioProviderFrame audioProvider::provide(int frames)
{
  audioProviderFrame result;
  result.left  = std::vector<float>(frames);
  result.right = std::vector<float>(frames);
  for (int i = 0; i < frames; ++i)
  {
    stereoSample sample;
    if (!readSamples.pop(sample))
    {
      sample.left = 0.0f;
      sample.right = 0.0f;
      // TODO log this
    }
    result.left.at(i)  = sample.left;
    result.right.at(i) = sample.right;
  }
  return result;
}

void audioProvider::readCallback(soundio::inStream& stream, int minFrames, int maxFrames)
{
  (void)minFrames;

  int framesToRead = maxFrames;

  while (true)
  {
    int framesRead = framesToRead;
    bool success = stream.beginRead(framesRead);

    if (!framesRead)
      break;

    std::vector<stereoSample> tempSamples(framesRead);

    // fill tempSamples
    if (!success)
    {
      // TODO internal frame drop, emit silence
      for (unsigned i = 0; i < framesRead; ++i)
      {
        tempSamples[i].left  = 0;
        tempSamples[i].right = 0;
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
      // TODO frame drop
    }

    stream.endRead();
    framesToRead -= framesRead;
    if (framesToRead <= 0)
      break;
  }
}
