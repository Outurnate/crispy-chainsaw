#include <optional>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "SceneManager.hpp"
#include "Application.hpp"
#include "BlackHoleScene.hpp"

int main()
{
  auto defaultLogger = spdlog::stdout_color_mt("default");
  auto ogreLogger = spdlog::stdout_color_mt("ogre");
  spdlog::set_default_logger(defaultLogger);
  //spdlog::register_logger(ogreLogger);

  /*SceneManager scene;
  scene.registerScene<BlackHoleScene>();

  scene.run();*/

  Application().run();

  return 0;
}
