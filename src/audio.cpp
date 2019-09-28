#include "audioStream.h"
#include "circleSpectrumScene.hpp"
#include "gl.hpp"

static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
    int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
  audioStream s;
  s.loadFile(std::string("/home/joseph/eclipse/audio_bak/Debug/test.wav"));
  s.start();

  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_SAMPLES, 4);

  {
    GLFWWindowPtr window(
        glfwCreateWindow(640, 480, "death by cold fries", NULL, NULL));
    if (!window)
    {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window.get(), key_callback);

    glfwMakeContextCurrent(window.get());
    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
      fprintf(stderr, "Failed to initialize OpenGL loader!\n");
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    glEnable(GL_MULTISAMPLE);

    circleSpectrumScene scene;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    int display_w, display_h;

    while (!glfwWindowShouldClose(window.get()))
    {
      glfwPollEvents();
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();

      ImGui::NewFrame();

      s.renderImGui();
      scene.update(s);

      ImGui::Render();

      glfwGetFramebufferSize(window.get(), &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      scene.render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window.get());
    }
  }

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
