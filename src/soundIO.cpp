#include "soundIO.hpp"

#include <new>
#include <fmt/format.h>

#define SOUNDIO_WRAP(X) if(int err=X;err)throw soundioException(err);

soundio::soundioException::soundioException(int err)
  : err(err)
{
}

soundio::soundioException::~soundioException()
{
}

const char* soundio::soundioException::what() const noexcept
{
  return soundio_strerror(err);
}

soundio::system::system()
{
  obj = soundio_create();

  if (!obj)
    throw std::bad_alloc();

  SOUNDIO_WRAP(soundio_connect(obj))

  soundio_flush_events(obj);
}

soundio::system::~system()
{
  soundio_destroy(obj);
}

void soundio::system::waitEvents()
{
  soundio_wait_events(obj);
}

void soundio::system::flushEvents()
{
  soundio_flush_events(obj);
}

void soundio::system::wakeUp()
{
  soundio_wakeup(obj);
}

soundio::outputDevice::outputDevice(soundio::system& system)
{
  int defaultIndex = soundio_default_output_device_index(system.obj);

  if (defaultIndex < 0)
    throw std::runtime_error("no output device found");

  if (obj = soundio_get_output_device(system.obj, defaultIndex); !obj)
    throw std::bad_alloc();
}

soundio::outputDevice::~outputDevice()
{
  soundio_device_unref(obj);
}

const std::string soundio::outputDevice::getName() const
{
  return obj->name;
}

soundio::inputDevice::inputDevice(soundio::system& system)
{
  int defaultIndex = soundio_default_input_device_index(system.obj);

  if (defaultIndex < 0)
    throw std::runtime_error("no input device found");

  if (obj = soundio_get_input_device(system.obj, defaultIndex); !obj)
    throw std::bad_alloc();
}

soundio::inputDevice::~inputDevice()
{
  soundio_device_unref(obj);
}

const std::string soundio::inputDevice::getName() const
{
  return obj->name;
}

soundio::outStream::outStream(outputDevice& device, const std::string& name, callback writeCallback, SoundIoFormat format, int sampleRate)
  : name(name), writeCallback(writeCallback)
{
  if (obj = soundio_outstream_create(device.obj); !obj)
    throw std::bad_alloc();

  obj->format = format;
  obj->sample_rate = sampleRate;
  obj->name = name.c_str();
  obj->userdata = this;

  obj->write_callback = [](SoundIoOutStream* obj, int frameCountMin, int frameCountMax)
      {
        outStream& stream = *reinterpret_cast<outStream*>(obj->userdata);
        stream.writeCallback(stream, frameCountMin, frameCountMax);
      };

  SOUNDIO_WRAP(soundio_outstream_open(obj))

  this->name = std::string(obj->name);
}

soundio::outStream::~outStream()
{
  soundio_outstream_destroy(obj);
}

int soundio::outStream::getChannels() const
{
  return obj->layout.channel_count;
}

const std::string soundio::outStream::getName() const
{
  return name;
}

void soundio::outStream::beginWrite(int& requestedFrameCount)
{
  SoundIoChannelArea* areasRaw;

  SOUNDIO_WRAP(soundio_outstream_begin_write(obj, &areasRaw, &requestedFrameCount))

  areas = ranges::v3::span<SoundIoChannelArea>(areasRaw, obj->layout.channel_count);

  this->frameCount = requestedFrameCount;
}

void soundio::outStream::endWrite()
{
  SOUNDIO_WRAP(soundio_outstream_end_write(obj))
}

void soundio::outStream::start()
{
  SOUNDIO_WRAP(soundio_outstream_start(obj))
}

void soundio::outStream::clearBuffer()
{
  SOUNDIO_WRAP(soundio_outstream_clear_buffer(obj))
}

std::chrono::duration<double> soundio::outStream::getLatency()
{
  double latency;
  SOUNDIO_WRAP(soundio_outstream_get_latency(obj, &latency));
  return std::chrono::duration<double>(latency);
}

soundio::inStream::inStream(inputDevice& device, callback readCallback, SoundIoFormat format, int sampleRate)
  : readCallback(readCallback)
{
  if (obj = soundio_instream_create(device.obj); !obj)
    throw std::bad_alloc();

  obj->format = format;
  obj->sample_rate = sampleRate;
  obj->userdata = this;

  obj->read_callback = [](SoundIoInStream* obj, int frameCountMin, int frameCountMax)
      {
        inStream& stream = *reinterpret_cast<inStream*>(obj->userdata);
        stream.readCallback(stream, frameCountMin, frameCountMax);
      };

  SOUNDIO_WRAP(soundio_instream_open(obj))
}

soundio::inStream::~inStream()
{
  soundio_instream_destroy(obj);
}

void soundio::inStream::start()
{
  SOUNDIO_WRAP(soundio_instream_start(obj))
}

bool soundio::inStream::beginRead(int& requestedFrameCount)
{
  SoundIoChannelArea* areasRaw;
  SOUNDIO_WRAP(soundio_instream_begin_read(obj, &areasRaw, &requestedFrameCount))

  if (!areasRaw)
    return false;

  areas = ranges::v3::span<SoundIoChannelArea>(areasRaw, obj->layout.channel_count);
  this->frameCount = requestedFrameCount;

  return true;
}

void soundio::inStream::endRead()
{
  SOUNDIO_WRAP(soundio_instream_end_read(obj))
}
