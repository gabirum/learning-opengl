#include <stdbool.h>
#include <stdio.h>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_NONE

#include <stb/stb_image.h>

#include "shader.h"

static void error_cb(int error, char const *desc);
static void framebuffer_size_cb(GLFWwindow *window, int width, int height);
static void key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);
static bool create_texture(char const *filename, unsigned int *texture);

int main(int argc, char const *argv[])
{
  glfwSetErrorCallback(error_cb);

  if (!glfwInit())
  {
    fprintf(stderr, "GLFW init failed\n");
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

  int retval = EXIT_SUCCESS;

  GLFWwindow *window = glfwCreateWindow(800, 600, "Triângulo", NULL, NULL);
  if (window == NULL)
  {
    fprintf(stderr, "GLFW create window failed\n");
    retval = EXIT_FAILURE;
    goto terminate;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);
  glfwSetKeyCallback(window, key_cb);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress))
  {
    fprintf(stderr, "GLAD init failed\n");
    retval = EXIT_FAILURE;
    goto destroy;
  }
  glfwSwapInterval(1000 / 75);

  shader_t *shader = shader_new("resources/shaders/shader.vert", "resources/shaders/shader.frag");
  if (shader == NULL)
  {
    retval = EXIT_FAILURE;
    goto destroy;
  }

  float vertices[] = {
      0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
      0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int vbo, vao, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int texture;
  if (!create_texture("resources/textures/container.jpg", &texture))
  {
    retval = EXIT_FAILURE;
    goto cleanup;
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window))
  {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    shader_use(shader);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &texture);
cleanup:
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
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

static bool create_texture(char const *filename, unsigned int *texture)
{
  unsigned int new_texture;
  glGenTextures(1, &new_texture);
  glBindTexture(GL_TEXTURE_2D, new_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
  if (data == NULL)
  {
    fprintf(stderr, "Error loading image texture");
    glDeleteTextures(1, &new_texture);
    return false;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  *texture = new_texture;
  return true;
}
