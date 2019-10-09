#include "audioStream.h"

#include "imgui.hpp"
#include <iostream>
#include <queue>
#include <complex>
#include <numeric>
#include <time.h>

audioStream::audioStream() :
    pos(0), analysisThread(&audioStream::analysis, this)
{
  portaudio::Device &device(portaudio::System::instance().defaultOutputDevice());

  portaudio::DirectionSpecificStreamParameters outParams(device,
      audioAnalyzer::CHANNELS, portaudio::FLOAT32, false, device.defaultLowOutputLatency(),
      NULL);
  portaudio::StreamParameters params(
      portaudio::DirectionSpecificStreamParameters::null(), outParams,
      audioAnalyzer::SAMPLE_RATE,
      FRAMES_PER_BUFFER,
      paClipOff);

  stream.reset(
      new portaudio::MemFunCallbackStream<audioStream>(params, *this,
          &audioStream::getSample));
}

audioStream::~audioStream()
{
}

void audioStream::loadFile(const std::string &fileURI)
{
  // TODO none of this is threadsafe...at all.....
  // we have to stop the stream
  // maybe resample the audio
  file.load(fileURI);
}

void audioStream::start()
{
  stream->start();
}
void audioStream::stop()
{
  stream->stop();
}

const bool audioStream::isPlaying() const
{
  return !stream->isStopped();
}

int audioStream::getSample(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags)
{
  assert(outputBuffer != NULL);
  assert(framesPerBuffer == FRAMES_PER_BUFFER);

  if ((pos + framesPerBuffer) > file.getNumSamplesPerChannel())
    return paComplete;

  float **out = static_cast<float**>(outputBuffer);
  for (unsigned i = 0; i < framesPerBuffer; ++i)
    for (unsigned channel = 0; channel < audioAnalyzer::CHANNELS; ++channel)
    {
      out[channel][i] = file.samples[channel % file.getNumChannels()][pos + i]; // if file has less channels, limit to CHANNELS, if CHANNELS is more than file channels, copy channel
      if (!analysisQueue[channel].push(out[channel][i]))
      {
        // i dunno...drop the rest of the frame i guess
      }
    }

  pos += framesPerBuffer;

  return paContinue;
}

void audioStream::analysis()
{
  struct timespec time;
  time.tv_sec = 0;
  time.tv_nsec = 500;

  std::array<std::queue<float>, analyzer::CHANNELS> window;

  while (true)
  {
    for (unsigned i = 0; i < FRAMES_PER_BUFFER; ++i)
      for (unsigned channel = 0; channel < analyzer::CHANNELS; ++channel)
      {
        float data;
        while (!analysisQueue[channel].pop(data))
          nanosleep(&time, NULL);
        window[channel].push(data);
      }

    if (std::any_of(window.begin(), window.end(), [](const std::queue<float> &i) 
    { return i.size() < analyzer::WINDOW_SIZE;}))
      continue; // we don't have enough samples to do FFT yet

    analyzer::audioSourceFrame sourceSample;
    for (unsigned channel = 0; channel < analyzer::CHANNELS; ++channel)
    {
      for (unsigned i = 0; i < analyzer::WINDOW_SIZE; ++i)
      {
        sourceSample[channel][i] = window[channel].front();
        window[channel].pop();
      }
    }

    analysisEngine.analyze(sourceSample);
  }
}

void audioStream::renderImGui() const
{
  std::array<float, analyzer::FFT_BINS> data;
  ImGui::PlotConfig conf;

  ImGui::Begin("Audio data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  for (unsigned i = 0; i < analyzer::FFT_BINS; ++i)
    data[i] = getLatestFrame().spectrum.at(i).magnitude;
  conf.values.ys = data.data();
  conf.values.count = analyzer::FFT_BINS;
  conf.scale.min = 0;
  conf.scale.max = 7.5f;
  conf.tooltip.show = true;
  conf.tooltip.format = "x=%.2f, y=%.2f";
  conf.grid_x.show = false;
  conf.grid_y.show = true;
  conf.frame_size = ImVec2(50 * 7, 75);
  conf.line_thickness = 2.f;
  ImGui::Plot("plot", conf);
  for (unsigned i = 0; i < analyzer::FFT_BINS; ++i)
    data[i] = getLatestFrame().spectrum.at(i).balance;
  conf.values.ys = data.data();
  conf.values.count = analyzer::FFT_BINS;
  conf.scale.min = -2.5f;
  conf.scale.max = 2.5f;
  conf.tooltip.show = true;
  conf.tooltip.format = "x=%.2f, y=%.2f";
  conf.grid_x.show = false;
  conf.grid_y.show = true;
  conf.frame_size = ImVec2(50 * 7, 75);
  conf.line_thickness = 2.f;
  ImGui::Plot("plot", conf);
  ImGui::End();
}

const audioStream::analyzer::audioAnalyzedFrame& audioStream::getLatestFrame() const
{
  return analysisEngine.getData();
}
