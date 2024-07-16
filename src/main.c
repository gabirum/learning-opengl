#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_NONE

#include <stb/stb_image.h>
#include <cglm/cglm.h>

#include "shader.h"

#define CLAMP(v, lo, hi) (v < lo ? lo : (v > hi ? hi : v))

static void error_cb(int error, char const *desc);
static void framebuffer_size_cb(GLFWwindow *window, int width, int height);
static void key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouse_cb(GLFWwindow * window, double xpos, double ypos);
static void scroll_cb(GLFWwindow * window, double xoff, double yoff);
static bool create_texture(char const *filename, unsigned int *texture, GLint color_format);

static int screen_width = 800;
static int screen_height = 600;
static float lastx = 400;
static float lasty = 300;

static float delta_time = 0.f;
static float last_frame = 0.f;

static bool is_first_mouse_enter = true;
static float fov = 45.f;
static float yaw = -90.f;
static float pitch = 0.f;

static vec3 camera_pos = {0.f, 0.f, 3.f};
static vec3 camera_front = {0.f, 0.f, -1.f};
static vec3 camera_up = {0.f, 1.f, 0.f};

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

  GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "Learning OpenGL", NULL, NULL);
  if (window == NULL)
  {
    fprintf(stderr, "GLFW create window failed\n");
    retval = EXIT_FAILURE;
    goto terminate;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);
  glfwSetKeyCallback(window, key_cb);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_cb);
  glfwSetScrollCallback(window, scroll_cb);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress))
  {
    fprintf(stderr, "GLAD init failed\n");
    retval = EXIT_FAILURE;
    goto destroy;
  }
  glfwSwapInterval(GLFW_TRUE);

  glEnable(GL_DEPTH_TEST);

  shader_t *shader = shader_new("resources/shaders/shader.vert", "resources/shaders/shader.frag");
  if (shader == NULL)
  {
    retval = EXIT_FAILURE;
    goto destroy;
  }

  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int texture1, texture2;
  if (!create_texture("resources/textures/container.jpg", &texture1, GL_RGB))
  {
    retval = EXIT_FAILURE;
    goto cleanup;
  }

  if (!create_texture("resources/textures/awesomeface.png", &texture2, GL_RGBA))
  {
    retval = EXIT_FAILURE;
    goto cleanup;
  }

  vec3 cube_positions[] = {
      {0.0f, 0.0f, 0.0f},
      {2.0f, 5.0f, -15.0f},
      {-1.5f, -2.2f, -2.5f},
      {-3.8f, -2.0f, -12.3f},
      {2.4f, -0.4f, -3.5f},
      {-1.7f, 3.0f, -7.5f},
      {1.3f, -2.0f, -2.5f},
      {1.5f, 2.0f, -2.5f},
      {1.5f, 0.2f, -1.5f},
      {-1.3f, 1.0f, -1.5},
  };

  shader_use(shader);
  shader_set_int(shader, "texture1", 0);
  shader_set_int(shader, "texture2", 1);

  while (!glfwWindowShouldClose(window))
  {
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    glClearColor(.2f, .3f, .3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    shader_use(shader);

    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_perspective(glm_rad(fov), ((float)screen_width) / ((float)screen_height), .1f, 100.f, projection);
    shader_set_mat4(shader, "projection", projection);

    mat4 view = GLM_MAT4_IDENTITY_INIT;
    vec3 center;
    glm_vec3_add(camera_pos, camera_front, center);
    glm_lookat(camera_pos, center, camera_up, view);
    shader_set_mat4(shader, "view", view);

    glBindVertexArray(vao);
    for (int i = 0; i < 10; i++)
    {
      mat4 model = GLM_MAT4_IDENTITY_INIT;
      glm_translate(model, cube_positions[i]);

      float angle = 20.f * i;
      glm_rotate(model, glm_rad(angle), (vec3){1.f, .3f, .5f});
      shader_set_mat4(shader, "model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &texture1);
  glDeleteTextures(1, &texture2);
cleanup:
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
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, width, height);
}

static void key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  float const camera_speed = 2.5f * delta_time;
  vec3 scale, result;
  if (key == GLFW_KEY_W && (action & (GLFW_PRESS | GLFW_REPEAT)))
  {
    glm_vec3_scale(camera_front, camera_speed, scale);
    glm_vec3_add(camera_pos, scale, result);
    memcpy(camera_pos, result, sizeof(result));
  }

  if (key == GLFW_KEY_S && (action & (GLFW_PRESS | GLFW_REPEAT)))
  {
    glm_vec3_scale(camera_front, camera_speed, scale);
    glm_vec3_sub(camera_pos, scale, result);
    memcpy(camera_pos, result, sizeof(result));
  }

  vec3 cross;
  if (key == GLFW_KEY_A && (action & (GLFW_PRESS | GLFW_REPEAT)))
  {
    glm_cross(camera_front, camera_up, cross);
    glm_normalize(cross);
    glm_vec3_scale(cross, camera_speed, scale);
    glm_vec3_sub(camera_pos, scale, result);
    memcpy(camera_pos, result, sizeof(result));
  }

  if (key == GLFW_KEY_D && (action & (GLFW_PRESS | GLFW_REPEAT)))
  {
    glm_cross(camera_front, camera_up, cross);
    glm_normalize(cross);
    glm_vec3_scale(cross, camera_speed, scale);
    glm_vec3_add(camera_pos, scale, result);
    memcpy(camera_pos, result, sizeof(result));
  }
}

void mouse_cb(GLFWwindow *window, double xpos, double ypos)
{
  if (is_first_mouse_enter) {
    lastx = xpos;
    lasty = ypos;
    is_first_mouse_enter = false;
  }

  float xoff = xpos - lastx;
  float yoff = lasty - ypos;
  lastx = xpos;
  lasty = ypos;

  float const sensitivity = .1f;
  xoff *= sensitivity;
  yoff *= sensitivity;

  yaw += xoff;
  pitch += yoff;

  pitch = CLAMP(pitch, -89.f, 89.f);

  vec3 direction;
  direction[0] = cosf(glm_rad(yaw)) * cosf(glm_rad(pitch));
  direction[1] = sinf(glm_rad(pitch));
  direction[2] = sinf(glm_rad(yaw)) * cosf(glm_rad(pitch));
  glm_normalize(direction);
  memcpy(camera_front, direction, sizeof(direction));
}

void scroll_cb(GLFWwindow *window, double xoff, double yoff)
{
  fov -= (float) yoff;
  fov = CLAMP(fov, 1.f, 70.f);
}

static bool create_texture(char const *filename, unsigned int *texture, GLint color_format)
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

  glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  *texture = new_texture;
  return true;
}
