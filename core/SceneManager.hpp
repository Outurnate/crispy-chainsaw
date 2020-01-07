#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <vector>
#include <mutex>
#include <boost/circular_buffer.hpp>
#include <range/v3/view/any_view.hpp>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>

#include "AudioSystem.hpp"
#include "AudioEngine.hpp"
#include "ConfigurationManager.hpp"

class Scene
{
public:
  Scene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet);
  virtual ~Scene();

  virtual void update(double delta) = 0;
  virtual void updateAudio(const FFTSpectrumData& audioFrame) = 0;
  virtual void initialize() = 0;
  virtual Ogre::Camera& getCamera() = 0;

  void setSceneManager(Ogre::SceneManager& sceneManager);

  const std::string displayName, name;
protected:
  Ogre::SceneManager* sceneManager;
};

class SceneManager
{
public:
  SceneManager();

  void frame(double delta);
  ConfigurationManager::OptionSet& getOptionSet();
  void setScene(size_t index);
  void setRoot(Ogre::Root& root, Ogre::Viewport& viewport);
  void notifyResize();
private:
  template<typename T>
  void registerScene(const std::string& displayName, const std::string& name) { scenes.emplace_back(new T(displayName, name, optionSet)); }
  void updateAudio(const FFTSpectrumData& audioFrame);

  ConfigurationManager::OptionSet optionSet;
  Ogre::Root* root;
  Ogre::Viewport* viewport;
  std::vector<std::unique_ptr<Scene> > scenes;
  size_t currentScene;

  boost::circular_buffer<float> frameDeltas;

  std::mutex frameAudioMutex;
  AudioEngine engine;
};

#endif
