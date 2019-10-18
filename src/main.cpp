#include <bigg.hpp>
#include <bx/allocator.h>

#include <memory>

#include "sceneManager.hpp"
#include "circleSpectrumScene.hpp"
#include "warpScene.hpp"

class audioVisualizationWindow : public bigg::Application
{
public:
  audioVisualizationWindow()
    : bigg::Application("death by cold fries"), scene(new sceneManager(this->mAllocator))
  {
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
    scene.reset(nullptr);

    return 0;
  }

private:
  std::unique_ptr<sceneManager> scene;
};

int main(int argc, char** argv)
{
  audioVisualizationWindow app;
  return app.run(argc, argv, bgfx::RendererType::OpenGL, BGFX_PCI_ID_NONE, 0, NULL, new bx::DefaultAllocator());
}
