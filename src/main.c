#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "shader.h"

static void error_cb(int error, char const *desc);
static void framebuffer_size_cb(GLFWwindow *window, int width, int height);
static void key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);

int main(int argc, char const *argv[])
{
  glfwSetErrorCallback(error_cb);

  if (!glfwInit())
  {
    fprintf(stderr, "GLFW init failed\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

  int retval = 0;

  GLFWwindow *window = glfwCreateWindow(800, 600, "Triângulo", NULL, NULL);
  if (window == NULL)
  {
    fprintf(stderr, "GLFW create window failed\n");
    retval = -1;
    goto terminate;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);
  glfwSetKeyCallback(window, key_cb);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress))
  {
    fprintf(stderr, "GLAD init failed\n");
    retval = -1;
    goto destroy;
  }
  glfwSwapInterval(1);

  shader_t *shader = shader_new("shader\\shader.vert", "shader\\shader.frag");
  if (shader == NULL)
  {
    retval = -1;
    goto destroy;
  }

  float vertices[] = {
      .9f, -.9f, 0.f, 0.f, 1.f, 0.f,
      -.9f, -.9f, 0.f, 0.f, 0.f, 1.f,
      0.f, .9f, 0.f, 1.f, 0.f, 0.f};

  unsigned int vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window))
  {
    glClearColor(.1f, .1f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_use(shader);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  shader_destroy(shader);

destroy:
  glfwDestroyWindow(window);
terminate:
  glfwTerminate();

  return retval;
}

static void error_cb(int error, char const *desc)
{
  fprintf(stderr, "Error (%d): %s\n", error, desc);
}

static void framebuffer_size_cb(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

static void key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
