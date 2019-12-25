#ifndef SOUNDIO_HPP
#define SOUNDIO_HPP

#include <soundio.h>
#include <string>
#include <chrono>
#include <functional>
#include <range/v3/span.hpp>
#include <range/v3/view/stride.hpp>

namespace SoundIO
{
  class SoundIOException : public std::exception
  {
  public:
    explicit SoundIOException(int err);
    virtual ~SoundIOException();
    virtual const char* what() const noexcept;
  private:
    int err;
  };

  class OutputDevice;
  class InputDevice;

  class System
  {
    friend class OutputDevice;
    friend class InputDevice;
  public:
    System();
    virtual ~System();

    System(const System&) = delete;
    System& operator=(const System&) = delete;

    System(System&&) = default;
    System& operator=(System&&) = default;

    void waitEvents();
    void flushEvents();
    void wakeUp();
  private:
    SoundIo* obj;
  };

  class OutStream;

  class OutputDevice
  {
    friend class OutStream;
  public:
    OutputDevice(System& System);
    virtual ~OutputDevice();

    OutputDevice(const OutputDevice&) = delete;
    OutputDevice& operator=(const OutputDevice&) = delete;

    OutputDevice(OutputDevice&&) = default;
    OutputDevice& operator=(OutputDevice&&) = default;

    const std::string getName() const;
  private:
    SoundIoDevice* obj;
  };

  class InStream;

  class InputDevice
  {
    friend class InStream;
  public:
    InputDevice(System& System);
    virtual ~InputDevice();

    InputDevice(const InputDevice&) = delete;
    InputDevice& operator=(const InputDevice&) = delete;

    InputDevice(InputDevice&&) = default;
    InputDevice& operator=(InputDevice&&) = default;

    const std::string getName() const;
  private:
    SoundIoDevice* obj;
  };

  class OutStream
  {
  public:
    typedef std::function<void(OutStream&, int, int)> Callback;

    OutStream(OutputDevice& device, const std::string& name, Callback writeCallback, SoundIoFormat format = SoundIoFormatFloat32NE, int sampleRate = 44100);
    virtual ~OutStream();

    OutStream(const OutStream&) = delete;
    OutStream& operator=(const OutStream&) = delete;

    OutStream(OutStream&&) = default;
    OutStream& operator=(OutStream&&) = default;

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
    Callback writeCallback;
    SoundIoOutStream* obj;
  };

  class InStream
  {
  public:
    typedef std::function<void(InStream&, int, int)> Callback;

    InStream(InputDevice& device, Callback readCallback, SoundIoFormat format = SoundIoFormatFloat32NE, int sampleRate = 44100);
    virtual ~InStream();

    InStream(const InStream&) = delete;
    InStream& operator=(const InStream&) = delete;

    InStream(InStream&&) = default;
    InStream& operator=(InStream&&) = default;

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
    Callback readCallback;
    SoundIoInStream* obj;
  };
};

#endif
