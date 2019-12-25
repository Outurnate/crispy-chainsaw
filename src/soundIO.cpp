#include "soundIO.hpp"

#include <new>
#include <fmt/format.h>

#define SOUNDIO_WRAP(X) if(int err=X;err)throw SoundIOException(err);

SoundIO::SoundIOException::SoundIOException(int err)
  : err(err)
{
}

SoundIO::SoundIOException::~SoundIOException()
{
}

const char* SoundIO::SoundIOException::what() const noexcept
{
  return soundio_strerror(err);
}

SoundIO::System::System()
{
  obj = soundio_create();

  if (!obj)
    throw std::bad_alloc();

  SOUNDIO_WRAP(soundio_connect(obj))

  soundio_flush_events(obj);
}

SoundIO::System::~System()
{
  soundio_destroy(obj);
}

void SoundIO::System::waitEvents()
{
  soundio_wait_events(obj);
}

void SoundIO::System::flushEvents()
{
  soundio_flush_events(obj);
}

void SoundIO::System::wakeUp()
{
  soundio_wakeup(obj);
}

SoundIO::OutputDevice::OutputDevice(SoundIO::System& system)
{
  int defaultIndex = soundio_default_output_device_index(system.obj);

  if (defaultIndex < 0)
    throw std::runtime_error("no output device found");

  if (obj = soundio_get_output_device(system.obj, defaultIndex); !obj)
    throw std::bad_alloc();
}

SoundIO::OutputDevice::~OutputDevice()
{
  soundio_device_unref(obj);
}

const std::string SoundIO::OutputDevice::getName() const
{
  return obj->name;
}

SoundIO::InputDevice::InputDevice(SoundIO::System& system)
{
  int defaultIndex = soundio_default_input_device_index(system.obj);

  if (defaultIndex < 0)
    throw std::runtime_error("no input device found");

  if (obj = soundio_get_input_device(system.obj, defaultIndex); !obj)
    throw std::bad_alloc();
}

SoundIO::InputDevice::~InputDevice()
{
  soundio_device_unref(obj);
}

const std::string SoundIO::InputDevice::getName() const
{
  return obj->name;
}

SoundIO::OutStream::OutStream(OutputDevice& device, const std::string& name, Callback writeCallback, SoundIoFormat format, int sampleRate)
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
        OutStream& stream = *reinterpret_cast<OutStream*>(obj->userdata);
        stream.writeCallback(stream, frameCountMin, frameCountMax);
      };

  SOUNDIO_WRAP(soundio_outstream_open(obj))

  this->name = std::string(obj->name);
}

SoundIO::OutStream::~OutStream()
{
  soundio_outstream_destroy(obj);
}

int SoundIO::OutStream::getChannels() const
{
  return obj->layout.channel_count;
}

const std::string SoundIO::OutStream::getName() const
{
  return name;
}

void SoundIO::OutStream::beginWrite(int& requestedFrameCount)
{
  SoundIoChannelArea* areasRaw;

  SOUNDIO_WRAP(soundio_outstream_begin_write(obj, &areasRaw, &requestedFrameCount))

  areas = ranges::v3::span<SoundIoChannelArea>(areasRaw, obj->layout.channel_count);

  this->frameCount = requestedFrameCount;
}

void SoundIO::OutStream::endWrite()
{
  SOUNDIO_WRAP(soundio_outstream_end_write(obj))
}

void SoundIO::OutStream::start()
{
  SOUNDIO_WRAP(soundio_outstream_start(obj))
}

void SoundIO::OutStream::clearBuffer()
{
  SOUNDIO_WRAP(soundio_outstream_clear_buffer(obj))
}

std::chrono::duration<double> SoundIO::OutStream::getLatency()
{
  double latency;
  SOUNDIO_WRAP(soundio_outstream_get_latency(obj, &latency));
  return std::chrono::duration<double>(latency);
}

SoundIO::InStream::InStream(InputDevice& device, Callback readCallback, SoundIoFormat format, int sampleRate)
  : readCallback(readCallback)
{
  if (obj = soundio_instream_create(device.obj); !obj)
    throw std::bad_alloc();

  obj->format = format;
  obj->sample_rate = sampleRate;
  obj->userdata = this;

  obj->read_callback = [](SoundIoInStream* obj, int frameCountMin, int frameCountMax)
      {
        InStream& stream = *reinterpret_cast<InStream*>(obj->userdata);
        stream.readCallback(stream, frameCountMin, frameCountMax);
      };

  SOUNDIO_WRAP(soundio_instream_open(obj))
}

SoundIO::InStream::~InStream()
{
  soundio_instream_destroy(obj);
}

void SoundIO::InStream::start()
{
  SOUNDIO_WRAP(soundio_instream_start(obj))
}

bool SoundIO::InStream::beginRead(int& requestedFrameCount)
{
  SoundIoChannelArea* areasRaw;
  SOUNDIO_WRAP(soundio_instream_begin_read(obj, &areasRaw, &requestedFrameCount))

  if (!areasRaw)
    return false;

  areas = ranges::v3::span<SoundIoChannelArea>(areasRaw, obj->layout.channel_count);
  this->frameCount = requestedFrameCount;

  return true;
}

void SoundIO::InStream::endRead()
{
  SOUNDIO_WRAP(soundio_instream_end_read(obj))
}
