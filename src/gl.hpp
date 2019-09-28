/*
 * gl.hpp
 *
 *  Created on: Sep 22, 2019
 *      Author: joseph
 */

#ifndef GL_HPP_
#define GL_HPP_

#include "imgui.hpp"
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

struct GLFWwindowDeleter
{
	void operator()(GLFWwindow* window) const
	{
		glfwDestroyWindow(window);
	}
};
typedef std::unique_ptr<GLFWwindow, GLFWwindowDeleter> GLFWWindowPtr;

#endif /* GL_HPP_ */
