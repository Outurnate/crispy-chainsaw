#ifndef SOUNDIO_HPP
#define SOUNDIO_HPP

#include <soundio.h>
#include <string>
#include <chrono>
#include <functional>
#include <range/v3/span.hpp>
#include <range/v3/view/stride.hpp>

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
  class inputDevice;

  class system
  {
    friend class outputDevice;
    friend class inputDevice;
  public:
    system();
    virtual ~system();

    system(const system&) = delete;
    system& operator=(const system&) = delete;

    system(system&&) = default;
    system& operator=(system&&) = default;

    void waitEvents();
    void flushEvents();
    void wakeUp();
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

  class inStream;

  class inputDevice
  {
    friend class inStream;
  public:
    inputDevice(system& system);
    virtual ~inputDevice();

    inputDevice(const inputDevice&) = delete;
    inputDevice& operator=(const inputDevice&) = delete;

    inputDevice(inputDevice&&) = default;
    inputDevice& operator=(inputDevice&&) = default;

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

    int getChannels() const;
    const std::string getName() const;
    void beginWrite(int& requestedFrameCount);
    void endWrite();
    void start();
    void clearBuffer();
    std::chrono::duration<double> getLatency(); // seconds

    template<typename T>
    auto channel(int index)
    {
      assert((areas[index].step % sizeof(T)) == 0);
      size_t strideT = areas[index].step / sizeof(T);
      return ranges::v3::span<T>(reinterpret_cast<T*>(areas[index].ptr), frameCount * strideT)
          | ranges::views::stride(strideT);
    }
  private:
    ranges::v3::span<SoundIoChannelArea> areas;
    int frameCount;
    std::string name;
    callback writeCallback;
    SoundIoOutStream* obj;
  };

  class inStream
  {
  public:
    typedef std::function<void(inStream&, int, int)> callback;

    inStream(inputDevice& device, callback readCallback, SoundIoFormat format = SoundIoFormatFloat32NE, int sampleRate = 48000);
    virtual ~inStream();

    inStream(const inStream&) = delete;
    inStream& operator=(const inStream&) = delete;

    inStream(inStream&&) = default;
    inStream& operator=(inStream&&) = default;

    bool beginRead(int& requestedFrameCount);
    void endRead();
    void start();

    template<typename T>
    auto channel(int index)
    {
      assert((areas[index].step % sizeof(T)) == 0);
      size_t strideT = areas[index].step / sizeof(T);
      return ranges::v3::span<T>(reinterpret_cast<T*>(areas[index].ptr), frameCount * strideT)
          | ranges::views::stride(strideT);
    }
  private:
    ranges::v3::span<SoundIoChannelArea> areas;
    int frameCount;
    callback readCallback;
    SoundIoInStream* obj;
  };
};

#endif
