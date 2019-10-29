#ifndef SOUNDIO_HPP
#define SOUNDIO_HPP

#include <soundio.h>
#include <string>
#include <functional>
#include <vector>
#include <range/v3/span.hpp>

namespace soundio
{
  class soundioException : public std::exception
  {
  public:
    explicit soundioException(int err);
    virtual ~soundioException();
    virtual const char* what() const noexcept;
  private:
    int err;
  };

  class outputDevice;
  class system
  {
    friend class outputDevice;
  public:
    system();
    virtual ~system();

    system(const system&) = delete;
    system& operator=(const system&) = delete;

    system(system&&) = default;
    system& operator=(system&&) = default;
  private:
    SoundIo* obj;
  };

  class outStream;

  class outputDevice
  {
    friend class outStream;
  public:
    outputDevice(system& system);
    virtual ~outputDevice();

    outputDevice(const outputDevice&) = delete;
    outputDevice& operator=(const outputDevice&) = delete;

    outputDevice(outputDevice&&) = default;
    outputDevice& operator=(outputDevice&&) = default;

    const std::string getName() const;
  private:
    SoundIoDevice* obj;
  };

  class outStream
  {
  public:
    typedef std::function<void(outStream&, int, int)> callback;

    outStream(outputDevice& device, const std::string& name, callback writeCallback, SoundIoFormat format = SoundIoFormatFloat32NE, int sampleRate = 48000);
    virtual ~outStream();

    outStream(const outStream&) = delete;
    outStream& operator=(const outStream&) = delete;

    outStream(outStream&&) = default;
    outStream& operator=(outStream&&) = default;

    const int getChannels() const;
    const std::string getName() const;
    void beginWrite(int& frameCount);
    ranges::v3::span<char> channel(int index);
    void endWrite();
  private:
    std::vector<SoundIoChannelArea*> areas;
    std::string name;
    callback writeCallback;
    SoundIoOutStream* obj;
  };
};

#endif
