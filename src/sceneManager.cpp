#include "sceneManager.hpp"

using namespace std::placeholders;

Scene::Scene() {}
Scene::~Scene() {}

SceneManager::SceneManager()
  : viewer(),
    scenes(),
    currentScene(0),
    frameDeltas(256, 0.0f),
    frameAudioMutex(),
    engine(std::bind(&SceneManager::updateAudio, this, _1))
{
}

void SceneManager::run()
{
  viewer.realize();
  setScene(0);
  double lastTime = viewer.elapsedTime();

  while (!viewer.done())
  {
    double now = viewer.elapsedTime();
    double delta = now - lastTime;

    {
      std::lock_guard lock(frameAudioMutex);

      frameDeltas.push_back(delta);
      scenes[currentScene]->update(delta);
    }

    viewer.frame();
    lastTime = now;
  }
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
