/*
 * gl.hpp
 *
 *  Created on: Sep 22, 2019
 *      Author: joseph
 */

#ifndef GL_HPP_
#define GL_HPP_

/*#include "imgui.hpp"
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <vector>
#include <boost/core/noncopyable.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <tuple>

///////////////////////////////////////////////////////////////////////////////

struct GLFWwindowDeleter
{
  void operator()(GLFWwindow *window) const
  {
    glfwDestroyWindow(window);
  }
};
typedef std::unique_ptr<GLFWwindow, GLFWwindowDeleter> GLFWWindowPtr;

///////////////////////////////////////////////////////////////////////////////

class GLuintMGR : private boost::noncopyable
{
public:
  GLuintMGR(GLuint id)
    : id(id) { }
  virtual ~GLuintMGR() { }

  operator const GLuint() { return id; }
protected:
  GLuint id;
};

class GLShader : public GLuintMGR
{
public:
  GLShader(GLenum shaderType, const std::string& source)
    : GLuintMGR(glCreateShader(shaderType))
  {
    GLint result = GL_FALSE;
    int logLength;

    const char* sourcePtr = source.c_str();
    glShaderSource(id, 1, &sourcePtr, NULL);
    glCompileShader(id);

    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
      std::vector<char> errorMessage(logLength + 1);
      glGetShaderInfoLog(id, logLength, NULL, &errorMessage[0]);
      printf("%s\n", &errorMessage[0]);
    }
  }

  virtual ~GLShader() override { glDeleteShader(id); }
};

class GLProgram : public GLuintMGR
{
public:
  GLProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) : GLuintMGR(glCreateProgram())
  {
    GLint result = GL_FALSE;
    int logLength;

    GLShader vertexShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLShader fragmentShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);

    /*glBindAttribLocation(id, 0, "in_Position");
    glBindAttribLocation(id, 1, "in_Color");*//*

    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &result);
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
      std::vector<char> ProgramErrorMessage(logLength+1);
      glGetProgramInfoLog(id, logLength, NULL, &ProgramErrorMessage[0]);
      printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(id, vertexShader);
    glDetachShader(id, fragmentShader);
  }

  virtual ~GLProgram() override { glDeleteProgram(id); }

  void use()
  {
    glUseProgram(id);
  }
};

template<typename T>
class GLModelData
{
public:
  GLModelData() {}
  virtual ~GLModelData() {}

  std::tuple<T&, glm::vec4&> operator[](const size_t i)
  {
    return std::make_tuple(position[i], color[i]);
  }

  const T* const positionData() const
  {
    return position.data();
  }

  const glm::vec4* const colorData() const
  {
    return color.data();
  }

  void pushBack(const T& position, const glm::vec4 color)
  {
    this->position.push_back(position);
    this->color.push_back(color);
  }

  const size_t size() const
  {
    assert(position.size() == color.size());
    return position.size();
  }
private:
  std::vector<T> position;
  std::vector<glm::vec4> color;
};

typedef GLModelData<glm::vec3> GLModelData3;
typedef GLModelData<glm::vec2> GLModelData2;*/

#endif /* GL_HPP_ */
