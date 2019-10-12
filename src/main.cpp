#include <bigg.hpp>
#include <bx/allocator.h>

#include "sceneManager.hpp"
#include "circleSpectrumScene.hpp"

class audioVisualizationWindow : public bigg::Application
{
public:
  audioVisualizationWindow()
    : bigg::Application("death by cold fries")
  {
  }

  void initialize(int argc, char** argv) override
  {
    init();

    scene.registerScene<circleSpectrumScene>("Circle Spectrum");
    scene.setScene("Circle Spectrum");
  }

private:
  void onReset()
  {
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()));
  }

  void update(float delta)
  {
    bgfx::touch(0);
    scene.update(delta, float(getWidth()), float(getHeight()));
  }

  sceneManager scene;
};

int main(int argc, char** argv)
{
  audioVisualizationWindow app;
  return app.run(argc, argv, bgfx::RendererType::OpenGL, BGFX_PCI_ID_NONE, 0, NULL, new bx::DefaultAllocator());
}
