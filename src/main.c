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
#include "camera.h"

static void _error_cb(int error, char const *desc);
static void _framebuffer_size_cb(GLFWwindow *window, int width, int height);
static void _key_cb(GLFWwindow *window, int key, int scancode, int action, int mods);
static void _mouse_cb(GLFWwindow *window, double xpos, double ypos);
static void _scroll_cb(GLFWwindow *window, double xoff, double yoff);
static bool _create_texture(char const *filename, GLuint *texture);

#define DEFAULT_SCR_W 1280
#define DEFAULT_SCR_H 720

static int screen_width = DEFAULT_SCR_W;
static int screen_height = DEFAULT_SCR_H;
static float lastx = DEFAULT_SCR_W / 2;
static float lasty = DEFAULT_SCR_H / 2;

static float const CUBE_VERTICES[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

static float frame_time = 0.f;
static float last_frame = 0.f;

static bool is_first_mouse_enter = true;

static camera_t camera;

static vec3 light_pos = {1.2f, 1.f, 2.f};

int main(int argc, char const *argv[])
{
  glfwSetErrorCallback(_error_cb);

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

  glfwSetFramebufferSizeCallback(window, _framebuffer_size_cb);
  glfwSetKeyCallback(window, _key_cb);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, _mouse_cb);
  glfwSetScrollCallback(window, _scroll_cb);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress))
  {
    fprintf(stderr, "GLAD init failed\n");
    retval = EXIT_FAILURE;
    goto destroy;
  }
  glfwSwapInterval(GLFW_TRUE);

  glEnable(GL_DEPTH_TEST);

  cam_init(
      (vec3){0.f, 0.f, 3.f},
      (vec3){0.f, 1.f, 0.f},
      (vec3){0.f, 0.f, -3.f},
      DEFAULT_YAW,
      DEFAULT_PITCH,
      DEFAULT_SPEED,
      DEFAULT_SENSE,
      DEFAULT_ZOOM,
      &camera);
  cam_set_constrain_pitch(&camera, true);

  shader_t cube_shader;
  if (!shader_init("resources/shaders/cube.vert", "resources/shaders/cube.frag", &cube_shader))
  {
    retval = EXIT_FAILURE;
    goto destroy;
  }

  shader_t light_cube_shader;
  if (!shader_init("resources/shaders/light.vert", "resources/shaders/light.frag", &light_cube_shader))
  {
    retval = EXIT_FAILURE;
    goto destroy_cube_shader;
  }

  GLuint vbo, cube_vao;
  glGenVertexArrays(1, &cube_vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);

  glBindVertexArray(cube_vao);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  GLuint light_cube_vao;
  glGenVertexArrays(1, &light_cube_vao);
  glBindVertexArray(light_cube_vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  GLuint diffuse_map;
  if (!_create_texture("resources/textures/container2.png", &diffuse_map))
  {
    retval = EXIT_FAILURE;
    goto clean_buffers;
  }

  GLuint specular_map;
  if (!_create_texture("resources/textures/container2_specular.png", &specular_map))
  {
    retval = EXIT_FAILURE;
    goto destroy_diffuse_tex;
  }

  shader_use(&cube_shader);
  shader_set_int(&cube_shader, "material.diffuse", 0);
  shader_set_int(&cube_shader, "material.specular", 1);

  while (!glfwWindowShouldClose(window))
  {
    float current_time = glfwGetTime();
    frame_time = current_time - last_frame;
    last_frame = current_time;

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* light_pos[0] = 1.f + sinf(current_time) * 2.f;
    light_pos[2] = 1.f + cosf(current_time) * 2.f; */

    shader_use(&cube_shader);

    shader_set_vec3(&cube_shader, "light.position", light_pos);

    vec3 cam_pos;
    cam_get_pos(&camera, cam_pos);
    shader_set_vec3(&cube_shader, "viewPos", cam_pos);

    shader_set_vec3(&cube_shader, "light.ambient", (vec3){.2f, .2f, .2f});
    shader_set_vec3(&cube_shader, "light.diffuse", (vec3){.5f, .5f, .5f});
    shader_set_vec3(&cube_shader, "light.specular", (vec3){1.f, 1.f, 1.f});

    shader_set_vec3(&cube_shader, "material.specular", (vec3){.5f, .5f, .5f});
    shader_set_float(&cube_shader, "material.shininess", 64.f);

    mat4 projection;
    glm_perspective(glm_rad(cam_get_zoom(&camera)), ((float)screen_width) / ((float)screen_height), .1f, 100.f, projection);
    shader_set_mat4(&cube_shader, "projection", projection);

    mat4 view;
    cam_get_view_matrix(&camera, view);
    shader_set_mat4(&cube_shader, "view", view);

    mat4 cube_model = GLM_MAT4_IDENTITY_INIT;
    shader_set_mat4(&cube_shader, "model", cube_model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map);

    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // light object
    shader_use(&light_cube_shader);
    shader_set_mat4(&light_cube_shader, "projection", projection);
    shader_set_mat4(&light_cube_shader, "view", view);

    mat4 light_cube_model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(light_cube_model, light_pos);
    glm_scale(light_cube_model, (vec3){.2f, .2f, .2f});
    shader_set_mat4(&light_cube_shader, "model", light_cube_model);

    glBindVertexArray(light_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &specular_map);
destroy_diffuse_tex:
  glDeleteTextures(1, &diffuse_map);
clean_buffers:
  glDeleteVertexArrays(1, &cube_vao);
  glDeleteVertexArrays(1, &light_cube_vao);
  glDeleteBuffers(1, &vbo);
destroy_light_shader:
  shader_deinit(&light_cube_shader);
destroy_cube_shader:
  shader_deinit(&cube_shader);
destroy:
  glfwDestroyWindow(window);
terminate:
  glfwTerminate();

  return retval;
}

static void _error_cb(int error, char const *desc)
{
  fprintf(stderr, "Error (%d): %s\n", error, desc);
}

static void _framebuffer_size_cb(GLFWwindow *window, int width, int height)
{
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, width, height);
}

static bool is_mouse_cursor_enabled = false;
static int const VALID_ACTION = GLFW_PRESS | GLFW_REPEAT;
static void _key_cb(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    if (is_mouse_cursor_enabled)
    {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    is_mouse_cursor_enabled = !is_mouse_cursor_enabled;
    return;
  }

  if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
  {
    if (is_mouse_cursor_enabled)
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      is_mouse_cursor_enabled = false;
    }

    return;
  }

  if (action & VALID_ACTION)
  {
    enum camera_mov_e mov;
    switch (key)
    {
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
      mov = CAMERA_FORWARD;
      break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
      mov = CAMERA_BACKWARD;
      break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
      mov = CAMERA_LEFT;
      break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
      mov = CAMERA_RIGHT;
      break;
    case GLFW_KEY_SPACE:
    case GLFW_KEY_PAGE_UP:
      mov = CAMERA_UP;
      break;
    case GLFW_KEY_LEFT_SHIFT:
    case GLFW_KEY_RIGHT_SHIFT:
    case GLFW_KEY_PAGE_DOWN:
      mov = CAMERA_DOWN;
      break;

    default:
      return;
    }

    cam_process_key(&camera, mov, frame_time);
  }
}

static void _mouse_cb(GLFWwindow *window, double xpos, double ypos)
{
  if (is_mouse_cursor_enabled)
  {
    return;
  }

  if (is_first_mouse_enter)
  {
    lastx = xpos;
    lasty = ypos;
    is_first_mouse_enter = false;
  }

  float xoff = xpos - lastx;
  float yoff = lasty - ypos;

  lastx = xpos;
  lasty = ypos;

  cam_process_mouse(&camera, xoff, yoff);
}

static void _scroll_cb(GLFWwindow *window, double xoff, double yoff)
{
  if (is_mouse_cursor_enabled)
  {
    return;
  }

  cam_process_scroll(&camera, yoff);
}

static bool _create_texture(char const *filename, GLuint *texture)
{
  GLuint new_texture;
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

  GLenum color_format;
  if (channels == 1)
  {
    color_format = GL_RED;
  }
  else if (channels == 3)
  {
    color_format = GL_RGB;
  }
  else if (channels == 4)
  {
    color_format = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  *texture = new_texture;
  return true;
}
