#include "circleSpectrumScene.hpp"

#include "audioEngine.hpp"

#include <algorithm>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GEOMETRY_PASS 0

circleSpectrumScene::circleSpectrumScene(resourceManager& resources)
  : points(2 * (rangedSize(spectrumRange::lowMidrange) + rangedSize(spectrumRange::midrange) + rangedSize(spectrumRange::upperMidrange))),
    vertexBuffer(2 * points, positionColorVertex { 0.0f, 0.0f, 0.0f, 0xff000000 }),
    indexBuffer(6 * points, 0),
    baseRadius(0.15f),
    circleWidth(0.01f),
    scene(resources)
{
  program = resources.getShader("colors");

  unsigned c = 0;
  for (unsigned i = 0; i < points; ++i)
  {
    uint16_t topLeft     = i;
    uint16_t bottomLeft  = points + i;
    uint16_t topRight    = (i + 1) % points;
    uint16_t bottomRight = points + ((i + 1) % points);

    indexBuffer[c++] = bottomLeft;
    indexBuffer[c++] = topRight;
    indexBuffer[c++] = topLeft;

    indexBuffer[c++] = bottomLeft;
    indexBuffer[c++] = bottomRight;
    indexBuffer[c++] = topRight;
  }

  circleVBO = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vertexBuffer.data(), sizeof(positionColorVertex) * vertexBuffer.size()), positionColorVertex::msLayout);
  circleEBO = bgfx::createIndexBuffer(bgfx::makeRef(indexBuffer.data(), sizeof(uint16_t) * indexBuffer.size()));

  bgfx::setDebug(BGFX_DEBUG_TEXT);
}

circleSpectrumScene::~circleSpectrumScene()
{
  deleteHandle(circleVBO);
  deleteHandle(circleEBO);
}

void circleSpectrumScene::onReset(uint32_t width, uint32_t height)
{
  bgfx::setViewClear(GEOMETRY_PASS, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));
  bgfx::reset(width, height, BGFX_RESET_MSAA_X16);
}

void circleSpectrumScene::updateAudio(const audioAnalyzedFrame& audioFrame)
{
  // begin lazy inefficient code TODO
  std::vector<float> left, right, combined;
  std::for_each(rangedBegin(audioFrame.spectrum, spectrumRange::lowMidrange), rangedEnd(audioFrame.spectrum, spectrumRange::upperMidrange), [&left, &right](audioPoint point) { left.push_back(point.getLeft()); right.push_back(point.getRight()); });
  std::reverse(right.begin(), right.end());
  for (float x : right)
    combined.push_back(x);
  for (float x : left)
    combined.push_back(x);
  // end lazy inefficient code TODO

  for (unsigned i = 0; i < points; ++i)
  {
    float radius = baseRadius + (combined[i] / 7.0f);
    float theta = float(i) / float(points) * (2 * M_PI);
    theta += M_PI / 2;

    glm::vec2 outerPoint = polarToRect(radius + circleWidth, theta);
    glm::vec2 innerPoint = polarToRect(baseRadius,           theta);
    vertexBuffer[i         ] = { outerPoint.x, outerPoint.y, 0.0f, 0x000000ff };
    vertexBuffer[i + points] = { innerPoint.x, innerPoint.y, 0.0f, 0xff0055ff };
  }

  bgfx::update(circleVBO, 0, bgfx::copy(vertexBuffer.data(), sizeof(positionColorVertex) * vertexBuffer.size()));
}

void circleSpectrumScene::update(double delta, float width, float height)
{
  float aspect = width / height;

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -50.0f, 50.0f);
  bgfx::setViewTransform(GEOMETRY_PASS, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(GEOMETRY_PASS, 0, 0, uint16_t(width), uint16_t(height));

  bgfx::touch(0);

  glm::mat4 mtx = glm::identity<glm::mat4>();
  mtx *= glm::scale(mtx, glm::vec3(aspect, aspect, 0.0f));
  bgfx::setTransform(&mtx[0][0]);
  bgfx::setVertexBuffer(0, circleVBO);
  bgfx::setIndexBuffer(circleEBO);
  bgfx::setState(0
      | BGFX_STATE_WRITE_RGB
      | BGFX_STATE_WRITE_A
      | BGFX_STATE_MSAA);
  bgfx::submit(GEOMETRY_PASS, program);
}
