#include <optional>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "sceneManager.hpp"
#include "circleSpectrumScene.hpp"

int main()
{
  auto console = spdlog::stdout_color_mt("console");
  spdlog::set_default_logger(console);

  SceneManager scene;
  scene.registerScene<BlackHoleScene>();

  scene.run();

  return 0;
}
