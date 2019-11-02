#include <bigg.hpp>
#include <bx/allocator.h>
#include <optional>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "sceneManager.hpp"
#include "circleSpectrumScene.hpp"
#include "warpScene.hpp"

class audioVisualizationWindow : public bigg::Application
{
public:
  audioVisualizationWindow()
    : bigg::Application("death by cold fries"), scene()
  {
    scene.emplace(this->mAllocator);
  }

  void initialize(int argc, char** argv) override
  {
    init();

    scene->registerScene<circleSpectrumScene>("Circle Spectrum");
    scene->registerScene<warpScene>("Warp");
  }

  void onReset() override
  {
    scene->onReset(getWidth(), getHeight());
  }

  void update(float delta) override
  {
    bgfx::touch(0);
    scene->update(delta, float(getWidth()), float(getHeight()));
  }

  int shutdown() override
  {
    scene.reset();

    return 0;
  }

private:
  std::optional<sceneManager> scene;
};

int main(int argc, char** argv)
{
  auto console = spdlog::stdout_color_mt("console");
  spdlog::set_default_logger(console);
  audioVisualizationWindow app;
  return app.run(argc, argv, bgfx::RendererType::OpenGL, BGFX_PCI_ID_NONE, 0, NULL, new bx::DefaultAllocator());
}
