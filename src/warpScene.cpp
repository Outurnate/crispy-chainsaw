#include "warpScene.hpp"

#include "gfxUtils.hpp"
#include "math.hpp"
#include "globals.hpp"

#include <functional>

void warpScene::resetStar(star& obj)
{
  obj.theta = rng() * (M_PI * 2.0f);
  obj.length = (rng() * 2.0f) + 0.5f;
  obj.radius = 0.0f;
  obj.speed = rng() + 1.0f;
}

#define fadeDistance 0.4f
#define baseSize 0.1f
#define bassAmplitude 2.0f
#define fieldSize 3.0f

warpScene::warpScene()
  : scene()
{
  for (star& obj : stars)
  {
    resetStar(obj);
    obj.radius = rng() * fieldSize;
  }

  for (glm::vec2& obj : staticStars)
  {
    obj.x = (rng() * fieldSize) - (fieldSize / 2);
    obj.y = (rng() * fieldSize) - (fieldSize / 2);
  }
}

void warpScene::renderStar(const star& obj)
{
}

void warpScene::renderStaticStar(const glm::vec2& coord)
{
}

void warpScene::update(double delta, float width, float height)
{
  for (glm::vec2& obj : staticStars)
    renderStaticStar(obj);

  for (star& obj : stars)
  {
    obj.radius += obj.speed * delta;
    if (obj.radius > fieldSize)
      resetStar(obj);
    renderStar(obj);
  }
}

void warpScene::updateAudio(const fftSpectrumData& audioFrame)
{
  (void)audioFrame;
}

void warpScene::onReset(uint32_t width, uint32_t height)
{
}
