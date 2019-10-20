#include "warpScene.hpp"

#include "gfxUtils.hpp"
#include "math.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <functional>
#include <numeric>

#define GEOMETRY_PASS 0

static positionColorTextureVertex cubeVertices[] =
{
  {-1.0f,  1.0f,  0.0f, 0x00ff00ff, 0.0f, 1.0f },
  { 1.0f,  1.0f,  0.0f, 0x00ff00ff, 1.0f, 1.0f },
  {-1.0f, -1.0f,  0.0f, 0x00ff00ff, 0.0f, 0.0f },
  { 1.0f, -1.0f,  0.0f, 0x00ff00ff, 1.0f, 0.0f },
};

static const uint16_t cubeTriList[] =
{
  0, 1, 2,
  1, 3, 2,
};

void warpScene::resetStar(star& obj)
{
  obj.theta = rng() * (M_PI * 2.0f);
  obj.length = (rng() * 2.0f) + 0.5f;
  obj.radius = 0.0f;
  obj.speed = rng() + 1.0f;
}

warpScene::warpScene(resourceManager& resources)
  : scene(resources), bassVolume(0.0f)
{
  program = resources.getShader("textures");
  spotTexture = resources.getTexture("spot");
  pewTexture = resources.getTexture("pew");
  testVBO = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), positionColorTextureVertex::msLayout);
  testEBO = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

  s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
  blendColor = bgfx::createUniform("blendColor", bgfx::UniformType::Vec4);

  for (star& obj : stars)
    resetStar(obj);

  for (glm::vec2& obj : staticStars)
  {
    obj.x = (rng() * 2.0f) - 1.0f;
    obj.y = (rng() * 2.0f) - 1.0f;
  }
}

warpScene::~warpScene()
{
}

#define fadeDistance 0.2f
#define baseSize 0.1f
#define bassAmplitude 2.0f

void warpScene::renderStar(const star& obj)
{
  glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, clamp(obj.radius - fadeDistance, 0.0f, 1.0f));
  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx = glm::translate(mtx, glm::vec3(obj.cartesian(), 1.0f));
  mtx *= glm::yawPitchRoll(0.0f, 0.0f, float(obj.theta + (M_PI / 1.0f)));
  mtx = glm::scale(mtx, glm::vec3(0.1f * obj.length, 0.005f, 1.0f));

  bgfx::setTransform(&mtx[0][0]);
  bgfx::setTexture(GEOMETRY_PASS, s_texColor, pewTexture, BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR);
  bgfx::setVertexBuffer(0, testVBO);
  bgfx::setIndexBuffer(testEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::setUniform(blendColor, &color, 1);
  bgfx::submit(GEOMETRY_PASS, program);
}

void warpScene::renderStaticStar(const glm::vec2& coord)
{
  glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx = glm::translate(mtx, glm::vec3(coord, 0.5f));
  mtx = glm::scale(mtx, glm::vec3(0.001f, 0.001f, 1.0f));

  bgfx::setTransform(&mtx[0][0]);
  bgfx::setTexture(GEOMETRY_PASS, s_texColor, spotTexture, BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR);
  bgfx::setVertexBuffer(0, testVBO);
  bgfx::setIndexBuffer(testEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::setUniform(blendColor, &color, 1);
  bgfx::submit(GEOMETRY_PASS, program);
}

void warpScene::update(double delta, float width, float height)
{
  glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -50.0f, 50.0f);
  bgfx::setViewTransform(GEOMETRY_PASS, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));

  bgfx::touch(0);

  for (glm::vec2& obj : staticStars)
    renderStaticStar(obj);

  for (star& obj : stars)
  {
    obj.radius += obj.speed * delta;
    if (obj.radius > 2.0f)
      resetStar(obj);
    renderStar(obj);
  }

  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx = glm::scale(mtx, glm::vec3(bassVolume + baseSize, bassVolume + baseSize, 1.0f));
  bgfx::setTransform(&mtx[0][0]);
  bgfx::setTexture(GEOMETRY_PASS, s_texColor, spotTexture, BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR);
  bgfx::setVertexBuffer(0, testVBO);
  bgfx::setIndexBuffer(testEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::setUniform(blendColor, &color, 1);
  bgfx::submit(GEOMETRY_PASS, program);
}

void warpScene::updateAudio(const audioAnalyzedFrame& audioFrame)
{
  float sum = std::accumulate(
      rangedBegin(audioFrame.spectrum, spectrumRange::subBass),
      rangedEnd(audioFrame.spectrum, spectrumRange::bass),
      0.0f,
      [](const float a, const audioPoint& b){ return a + b.magnitude; });
  float count = rangedSize(spectrumRange::subBass) + rangedSize(spectrumRange::bass);

  bassVolume = sum / count / bassAmplitude;
}

void warpScene::onReset(uint32_t width, uint32_t height)
{
  bgfx::setViewClear(GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));
  bgfx::reset(width, height, BGFX_RESET_MSAA_X16);
}
