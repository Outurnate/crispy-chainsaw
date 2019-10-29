#include "audioOutput.hpp"

using namespace std::placeholders;

audioOutput::audioOutput(soundio::system& system)
  : device(system), stream(device, "death by cold fries", std::bind(&audioOutput::callback, this, _1, _2, _3))
{
}

audioOutput::~audioOutput()
{
}

void audioOutput::callback(soundio::outStream& stream, int minFrames, int maxFrames)
{
}
