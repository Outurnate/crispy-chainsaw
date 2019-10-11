#include <bigg.hpp>

class audioVisualizationWindow : public bigg::Application
{
  void onReset()
  {
    bgfx::setViewClear( 0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xc0c0c0ff, 1.0f, 0 );
    bgfx::setViewRect( 0, 0, 0, uint16_t( getWidth() ), uint16_t( getHeight() ) );
  }

  void update( float dt )
  {
    bgfx::touch( 0 );
    ImGui::ShowDemoWindow();
  }

public:
  audioVisualizationWindow()
    : bigg::Application("death by cold fries") {}
};

int main( int argc, char** argv )
{
  audioVisualizationWindow app;
  return app.run( argc, argv );
}
