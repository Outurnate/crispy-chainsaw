#include "sceneManager.hpp"

#include <imgui.h>
#include <imgui_plot.h>

using namespace std::placeholders;

scene::scene() {}
scene::~scene() {}

sceneManager::sceneManager() : scenes(), currentScene(nullptr), engine(std::bind(&sceneManager::updateAudio, this, _1))
{
}

sceneManager::~sceneManager() {}

sceneManager::abstractSceneFactory::abstractSceneFactory() {}
sceneManager::abstractSceneFactory::~abstractSceneFactory() {}

inline void plotValues(const float* data, int count, float min, float max)
{
  ImGui::PlotConfig conf;
  conf.values.ys = data;
  conf.values.count = count;
  conf.scale.min = min;
  conf.scale.max = max;
  conf.tooltip.show = true;
  conf.tooltip.format = "x=%.2f, y=%.2f";
  conf.grid_x.show = false;
  conf.grid_y.show = true;
  conf.frame_size = ImVec2(50 * 7, 75);
  conf.line_thickness = 2.f;
  ImGui::Plot("plot", conf);
}

void sceneManager::update(double delta, float width, float height)
{
  if (currentScene)
    currentScene->update(delta, width, height);

  const float MARGIN = 10.0f;
  ImGuiIO& io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - MARGIN, MARGIN), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::Begin("Audio data", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

  std::array<float, audioSystem::FFT_BINS> data;

  for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
    data[i] = lastFrame.spectrum.at(i).magnitude;
  plotValues(data.data(), audioSystem::FFT_BINS, 0.0f, 1.0f);

  for (unsigned i = 0; i < audioSystem::FFT_BINS; ++i)
    data[i] = lastFrame.spectrum.at(i).balance;
  plotValues(data.data(), audioSystem::FFT_BINS, -2.0f, 2.0f);

  ImGui::Text("Estimated tempo: %.0f", lastFrame.tempo);

  audioAnalyzer::params params = engine.getParams();
  ImGui::SliderFloat("Alpha", &params.alpha, 0.0f, 1.0f);
  ImGui::SliderFloat("Gamma", &params.gamma, 0.0f, 10.0f);
  ImGui::SliderFloat("Scale", &params.scale, 0.0f, 1.0f);
  engine.setParams(params);

  ImGui::End();
}

void sceneManager::setScene(const std::string& name)
{
  currentScene.reset(scenes.at(name)->createScene());
}

void sceneManager::updateAudio(const audioAnalyzedFrame& frame)
{
  if (currentScene)
    currentScene->updateAudio(frame);

  lastFrame = frame;
}
