#include "sceneManager.hpp"

#include <imgui.h>
#include <string_view>
#include <fmt/format.h>

using namespace std::placeholders;

scene::scene() {}
scene::~scene() {}

sceneManager::sceneManager() :
    scenes(),
    currentScene(nullptr),
    engine(std::bind(&sceneManager::updateAudio, this, _1)),
    currentItem(0),
    frameDeltas(256, 0.0f)
{
}

sceneManager::~sceneManager() {}

sceneManager::abstractSceneFactory::abstractSceneFactory() {}
sceneManager::abstractSceneFactory::~abstractSceneFactory() {}

inline void plotValues(const float* data, const std::string& label, int count, float min, float max)
{
  ImGui::PlotLines(label.c_str(), data, count, 0, NULL, min, max, ImVec2(200, 40));
}

void sceneManager::update(double delta, float width, float height)
{
  std::lock_guard lock(frameAudioMutex);

  frameDeltas.push_back(delta);

  if (currentScene)
    currentScene->update(delta, width, height);

  const float MARGIN = 10.0f;
  ImGuiIO& io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - MARGIN, MARGIN), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::Begin("config", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

  if (ImGui::CollapsingHeader("Audio data"))
  {
    std::array<float, audioSystem::FFT_BINS> data;

    for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
      data[i] = lastFrame.at(i).magnitude;
    plotValues(data.data(), "magnitude", audioSystem::FFT_BINS, 0.0f, 1.0f);

    for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
      data[i] = lastFrame.at(i).balance;
    plotValues(data.data(), "balance",   audioSystem::FFT_BINS, -2.0f, 2.0f);

    audioEngine::params params = engine.getParams();
    ImGui::SliderFloat("Alpha",    &params.alpha,    0.0f, 1.0f);
    ImGui::SliderFloat("Gamma",    &params.gamma,    0.0f, 10.0f);
    ImGui::SliderFloat("Scale",    &params.scale,    0.0f, 1.0f);
    ImGui::SliderFloat("Exponent", &params.exponent, 1.0f, 5.0f);
    engine.setParams(params);
  }

  if (ImGui::CollapsingHeader("Scene Configuration"))
  {
    int previousItem = currentItem;
    struct func { static bool get(void* data, int idx, const char** outStr) { *outStr = (*reinterpret_cast<std::vector<std::string>* >(data))[idx].c_str(); return true; } };
    ImGui::Combo("Scene", &currentItem, &func::get, &sceneNames, sceneNames.size());
    if (previousItem != currentItem)
      setScene(currentItem);
  }

  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(MARGIN, MARGIN), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::Begin("stats", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

  struct func { static float get(void* data, int i) { return (*reinterpret_cast<boost::circular_buffer<float>*>(data))[i] * 1000.0f; } };
  ImGui::PlotHistogram(fmt::format("{0:#.2} ms per frame, {1} fps", delta * 1000.0f, (1.0f / delta)).c_str(), &func::get, &frameDeltas, 256, 0, NULL, 0.0f, 30.0f, ImVec2(0, 0));

  ImGui::End();
}

void sceneManager::setScene(const size_t& index)
{
  currentScene.reset(scenes.at(index)->createScene());
}

void sceneManager::updateAudio(const fftSpectrumData& frame)
{
  std::lock_guard lock(frameAudioMutex);

  if (currentScene)
    currentScene->updateAudio(frame);

  lastFrame = frame;
}

void sceneManager::onReset(uint32_t width, uint32_t height)
{
  if (currentScene)
    currentScene->onReset(width, height);
}
