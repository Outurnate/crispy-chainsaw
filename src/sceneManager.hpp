#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <memory>
#include <unordered_map>

#include "audioSystem.hpp"
#include "audioEngine.hpp"

class scene
{
public:
  scene();
  virtual ~scene();

  virtual void update(double delta, float width, float height) = 0;
  virtual void updateAudio(const audioAnalyzedFrame& audioFrame) = 0;
};

class sceneManager
{
public:
  sceneManager();
  virtual ~sceneManager();

  template<typename T>
  void registerScene(const std::string& name)
  {
    scenes.emplace(name, new sceneFactory<T>());
  }
  void update(double delta, float width, float height);
  void setScene(const std::string& name);
  void updateAudio(const audioAnalyzedFrame& frame);
private:
  class abstractSceneFactory
  {
  public:
    abstractSceneFactory();
    virtual ~abstractSceneFactory();

    virtual scene* createScene() const = 0;
  };

  template<typename T>
  class sceneFactory : public abstractSceneFactory
  {
  public:
    sceneFactory() {}
    ~sceneFactory() {}

    scene* createScene() const override
    {
      return new T();
    }
  };

  std::unordered_map<std::string, std::unique_ptr<abstractSceneFactory> > scenes;
  std::unique_ptr<scene> currentScene;
  audioEngine engine;
  audioAnalyzedFrame lastFrame;
};

#endif
