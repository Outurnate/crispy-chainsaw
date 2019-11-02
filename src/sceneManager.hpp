#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <memory>
#include <mutex>
#include <boost/circular_buffer.hpp>

#include "audioSystem.hpp"
#include "audioEngine.hpp"

#include "resourceManager.hpp"

class scene
{
public:
  scene(resourceManager& resources);
  virtual ~scene();

  virtual void update(double delta, float width, float height) = 0;
  virtual void updateAudio(const fftSpectrumData& audioFrame) = 0;
  virtual void onReset(uint32_t width, uint32_t height) = 0;
};

class sceneManager
{
public:
  sceneManager(bigg::Allocator& allocator);
  virtual ~sceneManager();

  template<typename T>
  void registerScene(const std::string& name)
  {
    scenes.emplace_back(new sceneFactory<T>());
    sceneNames.emplace_back(name);

    if (!currentScene)
      setScene(0);
  }
  void update(double delta, float width, float height);
  void updateAudio(const fftSpectrumData& frame);
  void onReset(uint32_t width, uint32_t height);
private:
  class abstractSceneFactory
  {
  public:
    abstractSceneFactory();
    virtual ~abstractSceneFactory();

    virtual scene* createScene(resourceManager& resources) const = 0;
  };

  template<typename T>
  class sceneFactory : public abstractSceneFactory
  {
  public:
    sceneFactory() {}
    ~sceneFactory() {}

    scene* createScene(resourceManager& resources) const override
    {
      return new T(resources);
    }
  };

  void setScene(const size_t& index);

  std::vector<std::unique_ptr<abstractSceneFactory> > scenes;
  std::vector<std::string> sceneNames;
  std::unique_ptr<scene> currentScene;
  std::mutex frameAudioMutex;
  int currentItem;
  audioEngine engine;
  fftSpectrumData lastFrame;
  resourceManager resources;
  boost::circular_buffer<float> frameDeltas;
};

#endif
