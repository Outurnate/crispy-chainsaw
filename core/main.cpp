#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Application.hpp"

int main()
{
  auto defaultLogger = spdlog::stdout_color_mt("default");
  auto ogreLogger = spdlog::stdout_color_mt("ogre");
  spdlog::set_default_logger(defaultLogger);

  Application().run();

  return 0;
}
