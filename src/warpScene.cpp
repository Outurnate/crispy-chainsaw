#include "warpScene.hpp"

#include "gfxUtils.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#define GEOMETRY_PASS 0

static positionColorTextureVertex cubeVertices[] =
{
  {-1.0f,  1.0f,  1.0f, 0xff000000, 0.0f,   512.0f },
  { 1.0f,  1.0f,  1.0f, 0xff0000ff, 512.0f, 512.0f },
  {-1.0f, -1.0f,  1.0f, 0xff00ff00, 0.0f,   0.0f   },
  { 1.0f, -1.0f,  1.0f, 0xff00ffff, 512.0f, 0.0f   },
  {-1.0f,  1.0f, -1.0f, 0xffff0000, 0.0f,   512.0f },
  { 1.0f,  1.0f, -1.0f, 0xffff00ff, 512.0f, 512.0f },
  {-1.0f, -1.0f, -1.0f, 0xffffff00, 0.0f,   0.0f   },
  { 1.0f, -1.0f, -1.0f, 0xffffffff, 512.0f, 0.0f   },
};

static const uint16_t cubeTriList[] =
{
  0, 1, 2, // 0
  1, 3, 2,
  4, 6, 5, // 2
  5, 6, 7,
  0, 2, 4, // 4
  4, 2, 6,
  1, 5, 3, // 6
  5, 7, 3,
  0, 4, 1, // 8
  4, 5, 1,
  2, 3, 6, // 10
  6, 3, 7,
};

warpScene::warpScene(resourceManager& resources)
  : scene(resources)
{
  program = resources.getShader("textures");
  spotTexture = resources.getTexture("spot");
  testVBO = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), positionColorTextureVertex::msLayout);
  testEBO = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

  s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
}

warpScene::~warpScene()
{
}

static float acctime = 0.0;
void warpScene::update(double delta, float width, float height)
{
  acctime += delta;
  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -50.0f, 50.0f);
  bgfx::setViewTransform(GEOMETRY_PASS, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));

  bgfx::setTexture(0, s_texColor, spotTexture);

  bgfx::touch(0);

  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx *= glm::yawPitchRoll(acctime, acctime, 0.0f);
  mtx *= glm::scale(mtx, glm::vec3(0.1f, 0.1f, 0.1f));
  bgfx::setTransform(&mtx[0][0]);
  bgfx::setVertexBuffer(0, testVBO);
  bgfx::setIndexBuffer(testEBO);
  bgfx::setState(0
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::submit(GEOMETRY_PASS, program);
}

void warpScene::updateAudio(const audioAnalyzedFrame& audioFrame)
{
}

void warpScene::onReset(uint32_t width, uint32_t height)
{
  bgfx::setViewClear(GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));
  bgfx::reset(width, height, BGFX_RESET_MSAA_X16);
}
