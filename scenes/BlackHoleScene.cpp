#include "BlackHoleScene.hpp"

#include <algorithm>
#include <range/v3/algorithm/for_each.hpp>

#include "Math.hpp"

BlackHoleScene::BlackHoleScene()
  : Scene(),
    points(2 * (spectrumSize(SpectrumRange::LowMidrange, SpectrumRange::UpperMidrange))),
    baseRadius(0.15f),
    circleWidth(0.01f)/*,
    vertexBuffer(2 * points),
    indexBuffer(6 * points)*/
{
  options.emplace_back("asdf", ConfigurationManager::OptionType::Real, 0.5);
  /*program = resources.getShader("colors");

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
  }*/

  //circleVBO = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vertexBuffer.data(), sizeof(positionColorVertex) * vertexBuffer.size()), positionColorVertex::msLayout);
  //circleEBO = bgfx::createIndexBuffer(bgfx::makeRef(indexBuffer.data(), sizeof(uint16_t) * indexBuffer.size()));

  //bgfx::setDebug(BGFX_DEBUG_TEXT);
}

ranges::v3::any_view<ConfigurationManager::Option&> BlackHoleScene::getOptions()
{
  return options;
}

void BlackHoleScene::show()
{
}

void BlackHoleScene::hide()
{
}

const std::string BlackHoleScene::getDisplayName() const
{
  return "";
}

const std::string BlackHoleScene::getName() const
{
  return "";
}

void BlackHoleScene::updateAudio(const FFTSpectrumData& audioFrame)
{
  // begin lazy inefficient code TODO
  std::vector<float> left, right, combined;
  ranges::for_each(
      spectrumView(audioFrame, SpectrumRange::LowMidrange, SpectrumRange::UpperMidrange),
      [&left, &right](AudioPoint point) { left.push_back(point.getChannel(Channel::Left)); right.push_back(point.getChannel(Channel::Right)); });
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

    (void)radius;

    /*glm::vec2 outerPoint = polarToRect(radius + circleWidth, theta);
    glm::vec2 innerPoint = polarToRect(baseRadius,           theta);
    vertexBuffer[i         ] = { outerPoint.x, outerPoint.y, 0.0f, 0x000000ff };
    vertexBuffer[i + points] = { innerPoint.x, innerPoint.y, 0.0f, 0xff0055ff };*/
  }

  //bgfx::update(circleVBO, 0, bgfx::copy(vertexBuffer.data(), sizeof(positionColorVertex) * vertexBuffer.size()));
}

void BlackHoleScene::update(double delta)
{
  (void)delta;
  /*float aspect = width / height;

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
  bgfx::submit(GEOMETRY_PASS, program);*/
}
