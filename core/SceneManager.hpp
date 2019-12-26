#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <vector>
#include <mutex>
#include <boost/circular_buffer.hpp>
#include <osgViewer/Viewer>

#include "AudioSystem.hpp"
#include "AudioEngine.hpp"

class Scene
{
public:
  Scene();
  virtual ~Scene();

  virtual const std::string getDisplayName() const = 0;
  virtual const std::string getName() const = 0;
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual void update(double delta) = 0;
  virtual void updateAudio(const FFTSpectrumData& audioFrame) = 0;
};

class SceneManager
{
public:
  SceneManager();

  template<typename T>
  void registerScene() { scenes.emplace_back(new T()); }
  void run();
private:
  void updateAudio(const FFTSpectrumData& audioFrame);
  void setScene(size_t index);

  osgViewer::Viewer viewer;
  std::vector<std::unique_ptr<Scene> > scenes;
  size_t currentScene;

  boost::circular_buffer<float> frameDeltas;

  std::mutex frameAudioMutex;
  AudioEngine engine;
};

#endif
