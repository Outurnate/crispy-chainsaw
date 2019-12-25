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

static positionColorNormalVertex cubeVertices[] =
{
  // 3357ff
  // Top
  {-1.0f,  1.0f,  1.0f, 0xff00aaff,  0.0f,  1.0f,  0.0f },
  { 1.0f,  1.0f,  1.0f, 0xff00aaff,  0.0f,  1.0f,  0.0f },
  {-1.0f,  1.0f, -1.0f, 0xff00aaff,  0.0f,  1.0f,  0.0f },
  { 1.0f,  1.0f, -1.0f, 0xff00aaff,  0.0f,  1.0f,  0.0f },

  // Bottom
  {-1.0f, -1.0f,  1.0f, 0xff0000ff,  0.0f, -1.0f,  0.0f },
  { 1.0f, -1.0f,  1.0f, 0xff0000ff,  0.0f, -1.0f,  0.0f },
  {-1.0f, -1.0f, -1.0f, 0xff0000ff,  0.0f, -1.0f,  0.0f },
  { 1.0f, -1.0f, -1.0f, 0xff0000ff,  0.0f, -1.0f,  0.0f },

  // Right
  { 1.0f,  1.0f, -1.0f, 0xffaaff00,  1.0f,  0.0f,  0.0f },
  { 1.0f,  1.0f,  1.0f, 0xffaaff00,  1.0f,  0.0f,  0.0f },
  { 1.0f, -1.0f, -1.0f, 0xffaaff00,  1.0f,  0.0f,  0.0f },
  { 1.0f, -1.0f,  1.0f, 0xffaaff00,  1.0f,  0.0f,  0.0f },

  // Left
  {-1.0f,  1.0f, -1.0f, 0xff00ff00, -1.0f,  0.0f,  0.0f },
  {-1.0f,  1.0f,  1.0f, 0xff00ff00, -1.0f,  0.0f,  0.0f },
  {-1.0f, -1.0f, -1.0f, 0xff00ff00, -1.0f,  0.0f,  0.0f },
  {-1.0f, -1.0f,  1.0f, 0xff00ff00, -1.0f,  0.0f,  0.0f },

  // Rear
  {-1.0f,  1.0f,  1.0f, 0xffff0000,  0.0f,  0.0f,  1.0f },
  { 1.0f,  1.0f,  1.0f, 0xffff0000,  0.0f,  0.0f,  1.0f },
  {-1.0f, -1.0f,  1.0f, 0xffff0000,  0.0f,  0.0f,  1.0f },
  { 1.0f, -1.0f,  1.0f, 0xffff0000,  0.0f,  0.0f,  1.0f },

  // Front
  {-1.0f,  1.0f, -1.0f, 0xffffaa00,  0.0f,  0.0f, -1.0f },
  { 1.0f,  1.0f, -1.0f, 0xffffaa00,  0.0f,  0.0f, -1.0f },
  {-1.0f, -1.0f, -1.0f, 0xffffaa00,  0.0f,  0.0f, -1.0f },
  { 1.0f, -1.0f, -1.0f, 0xffffaa00,  0.0f,  0.0f, -1.0f },
};

static const uint16_t cubeIndicies[] =
{
  2,  1,  0,
  2,  3,  1,

  4,  5,  6,
  5,  7,  6,

  10, 9,  8,
  10, 11, 9,

  12, 13, 14,
  13, 15, 14,

  16, 17, 18,
  17, 19, 18,

  22, 21, 20,
  22, 23, 21,
};

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
  textures = resources.getShader("textures");
  colors = resources.getShader("colors");
  lights = resources.getShader("lights");
  spotTexture = resources.getTexture("spot");
  pewTexture = resources.getTexture("pew");
  quadVBO = bgfx::createVertexBuffer(bgfx::makeRef(quadVertices, sizeof(quadVertices)), positionColorTextureVertex::msLayout);
  quadEBO = bgfx::createIndexBuffer(bgfx::makeRef(quadIndicies, sizeof(quadIndicies)));
  cubeVBO = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), positionColorNormalVertex::msLayout);
  cubeEBO = bgfx::createIndexBuffer(bgfx::makeRef(cubeIndicies, sizeof(cubeIndicies)));

  s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
  blendColor = bgfx::createUniform("blendColor", bgfx::UniformType::Vec4);
  light0 = bgfx::createUniform("light0", bgfx::UniformType::Vec4);

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

static float timeCounter = 0.0;

void warpScene::update(double delta, float width, float height)
{
  //glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  timeCounter += delta;

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::perspective(45.0f, width / height, 0.001f, 150.0f);
  bgfx::setViewTransform(GEOMETRY_PASS, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));

  bgfx::touch(0);

  for (glm::vec2& obj : staticStars)
    renderStaticStar(obj);

  for (star& obj : stars)
  {
    obj.radius += obj.speed * delta;
    if (obj.radius > fieldSize)
      resetStar(obj);
    renderStar(obj);
  }

  glm::mat4 cubeLoc = glm::identity<glm::mat4>();
  cubeLoc = glm::scale(cubeLoc, glm::vec3(0.1f, 0.1f, 0.1f));
  cubeLoc *= glm::yawPitchRoll(10.0f, 0.0f, 0.0f);

  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx *= cubeLoc;
  bgfx::setTransform(&mtx[0][0]);
  bgfx::setVertexBuffer(0, cubeVBO);
  bgfx::setIndexBuffer(cubeEBO);

  bgfx::setState(0
      | BGFX_STATE_CULL_CCW
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  glm::vec4 lightDirection = glm::vec4(sin(timeCounter), cos(timeCounter), 0.5f, 1.0f);
  bgfx::setUniform(light0, &lightDirection, 1);
  bgfx::submit(GEOMETRY_PASS, lights);
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
