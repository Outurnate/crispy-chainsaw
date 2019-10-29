#include "soundIO.hpp"

#include <new>

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

  if (int err = soundio_connect(obj); err)
    throw soundioException(err);

  soundio_flush_events(obj);
}

soundio::system::~system()
{
  soundio_destroy(obj);
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

  if (int err = soundio_outstream_open(obj); err)
    throw soundioException(err);

  this->name = std::string(obj->name);
}

soundio::outStream::~outStream()
{
  soundio_outstream_destroy(obj);
}

const int soundio::outStream::getChannels() const
{
  return obj->layout.channel_count;
}

const std::string soundio::outStream::getName() const
{
  return name;
}

void soundio::outStream::beginWrite(int& frameCount)
{
  areas.resize(obj->layout.channel_count);

  if (int err = soundio_outstream_begin_write(obj, areas.data(), &frameCount); err)
    throw soundioException(err);
}

ranges::v3::span<char> soundio::outStream::channel(int index)
{
  return ranges::v3::span<char>(areas[index]->ptr, areas[index]->step / sizeof(char));
}

void soundio::outStream::endWrite()
{
  if (int err = soundio_outstream_end_write(obj); err)
    throw soundioException(err);
}