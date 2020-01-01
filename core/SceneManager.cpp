#include "SceneManager.hpp"

using namespace std::placeholders;

Scene::Scene() {}
Scene::~Scene() {}

SceneManager::SceneManager()
  : scenes(),
    currentScene(0),
    frameDeltas(256, 0.0f),
    frameAudioMutex(),
    engine(std::bind(&SceneManager::updateAudio, this, _1))
{
}

void SceneManager::run()
{
  setScene(0);
  double lastTime = 0;//viewer.elapsedTime();

  while (true)
  {
    double now = 0;//viewer.elapsedTime();
    double delta = now - lastTime;

    {
      std::lock_guard lock(frameAudioMutex);

      frameDeltas.push_back(delta);
      scenes[currentScene]->update(delta);
    }

    //viewer.frame();
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
