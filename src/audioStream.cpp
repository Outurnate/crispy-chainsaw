#include "audioStream.h"

#include "imgui.hpp"
#include <iostream>
#include <queue>
#include <complex>
#include <numeric>
#include <time.h>
#include <fftwpp/Array.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#include <fftwpp/fftw++.h>
#pragma clang diagnostic pop

audioStream::audioStream() :
    pos(0), analysisThread(&audioStream::analysis, this)
{
  portaudio::Device &device(portaudio::System::instance().defaultOutputDevice());

  portaudio::DirectionSpecificStreamParameters outParams(device,
  CHANNELS, portaudio::FLOAT32, false, device.defaultLowOutputLatency(),
  NULL);
  portaudio::StreamParameters params(
      portaudio::DirectionSpecificStreamParameters::null(), outParams,
      SAMPLE_RATE,
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
    for (unsigned channel = 0; channel < CHANNELS; ++channel)
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

// 20-60 Hz sub bass
// 60-250 Hz bass
// 250-500 Hz low midrange
// 500-2000 Hz midrange
// 2000-4000 Hz upper midrange
// 4000-6000 Hz presence
// 6000-20000 Hz brilliance
// https://dlbeer.co.nz/articles/fftvis.html
// https://stackoverflow.com/a/20584591

void audioStream::analysis()
{
  fftwpp::fftw::maxthreads = get_max_threads();
  struct timespec time;
  time.tv_sec = 0;
  time.tv_nsec = 500;

  std::array<std::queue<float>, CHANNELS> window;

  while (true)
  {
    for (unsigned i = 0; i < FRAMES_PER_BUFFER; ++i)
      for (unsigned channel = 0; channel < CHANNELS; ++channel)
      {
        float data;
        while (!analysisQueue[channel].pop(data))
          nanosleep(&time, NULL);
        window[channel].push(data);
      }

    if (std::any_of(window.begin(), window.end(), [](const std::queue<float> &i) 
    { return i.size() < WINDOW_SIZE;}))
      continue; // we don't have enough samples to do FFT yet

    fftResult analyzedSample;

    // analyze a sample
    for (unsigned channel = 0; channel < CHANNELS; ++channel)
    {
      size_t alignment = sizeof(Complex);
      Array::array1<double> windowedSample(WINDOW_SIZE, alignment);

      for (unsigned i = 0; i < WINDOW_SIZE; ++i)
      {
        // apply window function to all samples to reduce spectral leakage
        double w = 0.5 * (1 - cos(2 * M_PI * i / (WINDOW_SIZE - 1)));
        windowedSample[i] = w * window[channel].front();
        window[channel].pop();
      }

      Array::array1<Complex> transformedSample(FFT_BINS, alignment);
      fftwpp::rcfft1d forward(WINDOW_SIZE, windowedSample, transformedSample);
      forward.fft(windowedSample, transformedSample);

      for (unsigned i = 0; i < FFT_BINS; ++i)
      {
        double d = sqrt(
            pow(transformedSample[i].real(), 2)
                + pow(transformedSample[i].imag(), 2));
        analyzedSample[channel].emplace_back(d);
      }
    }

    result.analyzedFFT.push_front(analyzedSample); // TODO thread safety
    result.update();
  }
}

const audioStream::fftResult audioStream::analysisResult::emptyResult =
{ std::vector<float>(FFT_BINS, 0.0f), std::vector<float>(FFT_BINS, 0.0f) };

audioStream::analysisResult::analysisResult() :
    analyzedFFT(3, emptyResult), movingAverageFFT(emptyResult), rangedMovingAverageFFT(
    {
    { spectrumRange::subBass, emptyResult },
    { spectrumRange::bass, emptyResult },
    { spectrumRange::lowMidrange, emptyResult },
    { spectrumRange::midrange, emptyResult },
    { spectrumRange::upperMidrange, emptyResult },
    { spectrumRange::presence, emptyResult },
    { spectrumRange::brilliance, emptyResult } }), rangedLatestResult(
    {
    { spectrumRange::subBass, emptyResult },
    { spectrumRange::bass, emptyResult },
    { spectrumRange::lowMidrange, emptyResult },
    { spectrumRange::midrange, emptyResult },
    { spectrumRange::upperMidrange, emptyResult },
    { spectrumRange::presence, emptyResult },
    { spectrumRange::brilliance, emptyResult } })
{
}

audioStream::analysisResult::~analysisResult()
{
}


const audioStream::analysisResult& audioStream::getResult() const
{
  return result;
}

/*
 * 			subBass,       // 20-60 Hz
 bass,          // 60-250 Hz
 lowMidrange,   // 250-500 Hz
 midrange,      // 500-2000 Hz
 upperMidrange, // 2000-4000 Hz
 presence,      // 4000-6000 Hz
 brilliance     // 6000-20000 Hz
 */

void splitFFTRanges(const audioStream::fftResult &source,
    std::unordered_map<spectrumRange,
        audioStream::fftResult> &dest)
{
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[subBass][channel] =
        std::vector<float>(source[channel].begin(),
            source[channel].begin() + subBassIndex);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[bass][channel] =
        std::vector<float>(source[channel].begin() + subBassIndex,
            source[channel].begin() + bassIndex);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[lowMidrange][channel] =
        std::vector<float>(source[channel].begin() + bassIndex,
            source[channel].begin() + lowMidrangeIndex);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[midrange][channel] =
        std::vector<float>(source[channel].begin() + lowMidrangeIndex,
            source[channel].begin() + midrangeIndex);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[upperMidrange][channel] =
        std::vector<float>(source[channel].begin() + midrangeIndex,
            source[channel].begin() + upperMidrangeIndex);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[presence][channel] =
        std::vector<float>(source[channel].begin() + upperMidrangeIndex,
            source[channel].begin() + presenceIndex);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
    dest[brilliance][channel] =
        std::vector<float>(source[channel].begin() + presenceIndex,
            source[channel].end());
}

void audioStream::analysisResult::update()
{
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
  {
    std::vector<float> movingSum(FFT_BINS, 0.0f);
    for (unsigned i = 0; i < FFT_BINS; ++i)
      for (const fftResult &result : analyzedFFT)
        movingSum[i] += result[channel][i];
    for (float &sample : movingSum)
      sample /= analyzedFFT.size();

    movingAverageFFT[channel] = movingSum;
  }

  splitFFTRanges(movingAverageFFT, rangedMovingAverageFFT);
  splitFFTRanges(analyzedFFT.front(), rangedLatestResult);
}

void audioStream::renderImGui() const
{
  ImGui::Begin("Audio data", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
  {
    ImGui::PlotConfig conf;
    conf.values.ys = result.analyzedFFT.front()[channel].data();
    conf.values.count = FFT_BINS;
    conf.scale.min = 0;
    conf.scale.max = 100;
    conf.tooltip.show = true;
    conf.tooltip.format = "x=%.2f, y=%.2f";
    conf.grid_x.show = false;
    conf.grid_y.show = true;
    conf.frame_size = ImVec2(50 * 7, 75);
    conf.line_thickness = 2.f;
    ImGui::Plot("plot", conf);
  }
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
  {
    ImGui::PlotConfig conf;
    conf.values.ys = result.movingAverageFFT[channel].data();
    conf.values.count = result.movingAverageFFT[channel].size();
    conf.scale.min = 0;
    conf.scale.max = 100;
    conf.tooltip.show = true;
    conf.tooltip.format = "x=%.2f, y=%.2f";
    conf.grid_x.show = false;
    conf.grid_y.show = true;
    conf.frame_size = ImVec2(50 * 7, 75);
    conf.line_thickness = 2.f;
    ImGui::Plot("plot", conf);
  }
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
  {
    for (unsigned i = 0; i != 7; ++i)
    {
      ImGui::PlotConfig conf;
      conf.values.ys = result.rangedLatestResult.at(
          spectrumRange(i))[channel].data();
      conf.values.count = result.rangedLatestResult.at(
          spectrumRange(i))[channel].size();
      conf.scale.min = 0;
      conf.scale.max = 100;
      conf.tooltip.show = true;
      conf.tooltip.format = "x=%.2f, y=%.2f";
      conf.grid_x.show = false;
      conf.grid_y.show = true;
      conf.frame_size = ImVec2(50, 75);
      conf.line_thickness = 2.f;
      ImGui::Plot("plot", conf);
      ImGui::SameLine();
    }
    ImGui::Separator();
  }
  for (unsigned channel = 0; channel < CHANNELS; ++channel)
  {
    for (unsigned i = 0; i != 7; ++i)
    {
      ImGui::PlotConfig conf;
      conf.values.ys = result.rangedMovingAverageFFT.at(
          spectrumRange(i))[channel].data();
      conf.values.count = result.rangedMovingAverageFFT.at(
          spectrumRange(i))[channel].size();
      conf.scale.min = 0;
      conf.scale.max = 100;
      conf.tooltip.show = true;
      conf.tooltip.format = "x=%.2f, y=%.2f";
      conf.grid_x.show = false;
      conf.grid_y.show = true;
      conf.frame_size = ImVec2(50, 75);
      conf.line_thickness = 2.f;
      ImGui::Plot("plot", conf);
      ImGui::SameLine();
    }
    ImGui::Separator();
  }
  ImGui::End();
}
