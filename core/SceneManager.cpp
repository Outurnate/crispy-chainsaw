#include "SceneManager.hpp"

#include <spdlog/spdlog.h>
#include <OgreShaderGenerator.h>
#include <OgreMaterialManager.h>
#include <OgreSGTechniqueResolverListener.h>

#include "BlackHoleScene.hpp"

using namespace std::placeholders;

Scene::Scene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet)
  : displayName(displayName),
    name(name) { (void)optionSet; }
Scene::~Scene() {}

void Scene::setSceneManager(Ogre::SceneManager& sceneManager)
{
  this->sceneManager = &sceneManager;
  this->initialize();
}

SceneManager::SceneManager()
  : optionSet(),
    scenes(),
    currentScene(0),
    frameDeltas(256, 0.0f),
    frameAudioMutex(),
    engine(std::bind(&SceneManager::updateAudio, this, _1))
{
  registerScene<BlackHoleScene>("Black Hole", "blackhole");
}

ConfigurationManager::OptionSet& SceneManager::getOptionSet()
{
  return optionSet;
}

void SceneManager::frame(double delta)
{
  std::lock_guard lock(frameAudioMutex);

  frameDeltas.push_back(delta);
  scenes[currentScene]->update(delta);
}

void SceneManager::setScene(size_t index)
{
  currentScene = index;
  viewport->setCamera(&scenes[currentScene]->getCamera());
  notifyResize();
}

void SceneManager::setRoot(Ogre::Root& root, Ogre::Viewport& viewport)
{
  this->root = &root;
  this->viewport = &viewport;

  viewport.setBackgroundColour(Ogre::ColourValue(0, 0, 0));

  if (Ogre::RTShader::ShaderGenerator::initialize())
  {
    Ogre::RTShader::ShaderGenerator::getSingletonPtr()->setShaderCachePath("");
    Ogre::MaterialManager::getSingleton().addListener(new OgreBites::SGTechniqueResolverListener(Ogre::RTShader::ShaderGenerator::getSingletonPtr()));
  }
  else
  {
    spdlog::get("ogre")->critical("Failed to initialize ShaderGenerator");
    return;
  }

  for (auto& scene : scenes)
  {
    Ogre::SceneManager& sceneManager = *root.createSceneManager("DefaultSceneManager");
    Ogre::RTShader::ShaderGenerator::getSingletonPtr()->addSceneManager(&sceneManager);

    scene->setSceneManager(sceneManager);
  }

  engine.start();
}

void SceneManager::updateAudio(const FFTSpectrumData& frame)
{
  std::lock_guard lock(frameAudioMutex);

  scenes[currentScene]->updateAudio(frame);
}

void SceneManager::notifyResize()
{
  viewport->getCamera()->setAspectRatio(Ogre::Real(viewport->getActualWidth() / viewport->getActualHeight()));
}
