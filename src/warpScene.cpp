#include "warpScene.hpp"

#include "gfxUtils.hpp"
#include "math.hpp"
#include "globals.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <functional>

#define GEOMETRY_PASS 0

static positionColorTextureVertex quadVertices[] =
{
  {-1.0f,  1.0f,  0.0f, 0x00ff00ff, 0.0f, 1.0f },
  { 1.0f,  1.0f,  0.0f, 0x00ff00ff, 1.0f, 1.0f },
  {-1.0f, -1.0f,  0.0f, 0x00ff00ff, 0.0f, 0.0f },
  { 1.0f, -1.0f,  0.0f, 0x00ff00ff, 1.0f, 0.0f },
};

static const uint16_t quadIndicies[] =
{
  0, 1, 2,
  1, 3, 2,
};

static positionColorVertex cubeVertices[] =
{
  {-1.0f,  1.0f,  1.0f, 0xff000000 },
  { 1.0f,  1.0f,  1.0f, 0xff0000ff },
  {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
  { 1.0f, -1.0f,  1.0f, 0xff00ffff },
  {-1.0f,  1.0f, -1.0f, 0xffff0000 },
  { 1.0f,  1.0f, -1.0f, 0xffff00ff },
  {-1.0f, -1.0f, -1.0f, 0xffffff00 },
  { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeIndicies[] =
{
  0, 1, 2,
  1, 3, 2,
  4, 6, 5,
  5, 6, 7,
  0, 2, 4,
  4, 2, 6,
  1, 5, 3,
  5, 7, 3,
  0, 4, 1,
  4, 5, 1,
  2, 3, 6,
  6, 3, 7,
};

void warpScene::resetStar(star& obj)
{
  obj.theta = rng() * (M_PI * 2.0f);
  obj.length = (rng() * 2.0f) + 0.5f;
  obj.radius = 0.0f;
  obj.speed = rng() + 1.0f;
}

#define fadeDistance 0.2f
#define baseSize 0.1f
#define bassAmplitude 2.0f
#define fieldSize 3.0f

warpScene::warpScene()
  : scene()
{
  textures = resources.getShader("textures");
  colors = resources.getShader("colors");
  spotTexture = resources.getTexture("spot");
  pewTexture = resources.getTexture("pew");
  quadVBO = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), positionColorTextureVertex::msLayout);
  quadEBO = bgfx::createIndexBuffer(bgfx::makeRef(quadIndicies, sizeof(quadIndicies)));
  cubeVBO = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), positionColorVertex::msLayout);
  cubeEBO = bgfx::createIndexBuffer(bgfx::makeRef(cubeIndicies, sizeof(cubeIndicies)));

  s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
  blendColor = bgfx::createUniform("blendColor", bgfx::UniformType::Vec4);

  for (star& obj : stars)
    resetStar(obj);

  for (glm::vec2& obj : staticStars)
  {
    obj.x = (rng() * fieldSize) - (fieldSize / 2);
    obj.y = (rng() * fieldSize) - (fieldSize / 2);
  }
}

warpScene::~warpScene()
{
}

void warpScene::renderStar(const star& obj)
{
  glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, clamp(obj.radius - fadeDistance, 0.0f, 1.0f));
  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx = glm::translate(mtx, glm::vec3(obj.cartesian(), 1.0f));
  mtx *= glm::yawPitchRoll(0.0f, 0.0f, float(obj.theta + (M_PI / 1.0f)));
  mtx = glm::scale(mtx, glm::vec3(0.1f * obj.length, 0.005f, 1.0f));

  bgfx::setTransform(&mtx[0][0]);
  bgfx::setTexture(GEOMETRY_PASS, s_texColor, pewTexture, BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR);
  bgfx::setVertexBuffer(0, quadVBO);
  bgfx::setIndexBuffer(quadEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::setUniform(blendColor, &color, 1);
  bgfx::submit(GEOMETRY_PASS, textures);
}

void warpScene::renderStaticStar(const glm::vec2& coord)
{
  glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx = glm::translate(mtx, glm::vec3(coord, 0.5f));
  mtx = glm::scale(mtx, glm::vec3(0.001f, 0.001f, 1.0f));

  bgfx::setTransform(&mtx[0][0]);
  bgfx::setTexture(GEOMETRY_PASS, s_texColor, spotTexture, BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR);
  bgfx::setVertexBuffer(0, quadVBO);
  bgfx::setIndexBuffer(quadEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::setUniform(blendColor, &color, 1);
  bgfx::submit(GEOMETRY_PASS, textures);
}

void warpScene::update(double delta, float width, float height)
{
  //glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::perspective(45.0f, width / height, 0.001f, 150.0f);
  bgfx::setViewTransform(GEOMETRY_PASS, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));

  bgfx::touch(0);

  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx = glm::scale(mtx, glm::vec3(0.1f, 0.1f, 0.1f));
  mtx *= glm::yawPitchRoll(0.5f, 0.5f, 0.0f);
  bgfx::setTransform(&mtx[0][0]);
  bgfx::setVertexBuffer(0, cubeVBO);
  bgfx::setIndexBuffer(cubeEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::submit(GEOMETRY_PASS, colors);

  for (glm::vec2& obj : staticStars)
    renderStaticStar(obj);

  for (star& obj : stars)
  {
    obj.radius += obj.speed * delta;
    if (obj.radius > fieldSize)
      resetStar(obj);
    renderStar(obj);
  }

  /*mtx = glm::identity<glm::mat4>();
  mtx = glm::scale(mtx, glm::vec3(bassVolume + baseSize, bassVolume + baseSize, 1.0f));
  bgfx::setTransform(&mtx[0][0]);
  bgfx::setTexture(GEOMETRY_PASS, s_texColor, spotTexture, BGFX_SAMPLER_U_MIRROR | BGFX_SAMPLER_V_MIRROR);
  bgfx::setVertexBuffer(0, quadVBO);
  bgfx::setIndexBuffer(quadEBO);

  bgfx::setState(0
      | BGFX_STATE_BLEND_ALPHA
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::setUniform(blendColor, &color, 1);
  bgfx::submit(GEOMETRY_PASS, textures);*/
}

void warpScene::updateAudio(const fftSpectrumData& audioFrame)
{
  (void)audioFrame;
  //bassVolume = spectrumAverage(audioFrame, spectrumRange::subBass, spectrumRange::bass) / bassAmplitude;
}

void warpScene::onReset(uint32_t width, uint32_t height)
{
  bgfx::setViewClear(GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));
  bgfx::reset(width, height, BGFX_RESET_MSAA_X16);
}
