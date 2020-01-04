#include "SceneManager.hpp"

#include "BlackHoleScene.hpp"

using namespace std::placeholders;

Scene::Scene() {}
Scene::~Scene() {}

SceneManager::SceneManager()
  : optionSet(),
    scenes(),
    currentScene(0),
    frameDeltas(256, 0.0f),
    frameAudioMutex(),
    engine(std::bind(&SceneManager::updateAudio, this, _1))
{
  registerScene<BlackHoleScene>();

  for (auto& scene : scenes)
  {
    auto options = scene->getOptions();
    for (auto& option : options)
      optionSet.registerOption(option);
  }
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
  scenes[currentScene]->hide();
  currentScene = index;
  scenes[currentScene]->show();
}

void SceneManager::updateAudio(const FFTSpectrumData& frame)
{
  std::lock_guard lock(frameAudioMutex);

  scenes[currentScene]->updateAudio(frame);
}
