#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#undef GLFW_INCLUDE_NONE

#include <stb_image.h>
#include <cglm/cglm.h>

#include "shader.h"
#include "camera.h"

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(*arr))

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

static vec3 CUBE_POSITIONS[] = {
    {0.0f, 0.0f, 0.0f},
    {2.0f, 5.0f, -15.0f},
    {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f},
    {2.4f, -0.4f, -3.5f},
    {-1.7f, 3.0f, -7.5f},
    {1.3f, -2.0f, -2.5f},
    {1.5f, 2.0f, -2.5f},
    {1.5f, 0.2f, -1.5f},
    {-1.3f, 1.0f, -1.5f}};

static vec3 POINT_LIGHT_POSITIONS[] = {
    {.7f, .2f, 2.f},
    {2.3f, -3.3f, -4.f},
    {-4.f, 2.f, -12.f},
    {0.f, 0.f, -3.f}};

#define SET_POINT_LIGHT(shader, index, array)                                             \
  do                                                                                      \
  {                                                                                       \
    shader_set_vec3(shader, "pointLights[" #index "].position", array[index]);            \
    shader_set_vec3(shader, "pointLights[" #index "].ambient", (vec3){.05f, .05f, .05f}); \
    shader_set_vec3(shader, "pointLights[" #index "].diffuse", (vec3){.8f, .8f, .8f});    \
    shader_set_vec3(shader, "pointLights[" #index "].specular", (vec3){1.f, 1.f, 1.f});   \
    shader_set_float(shader, "pointLights[" #index "].constant", 1.f);                    \
    shader_set_float(shader, "pointLights[" #index "].linear", .09f);                     \
    shader_set_float(shader, "pointLights[" #index "].quadratic", .032f);                 \
  } while (0)

static float frame_time = 0.f;
static float last_frame_time = 0.f;

static bool is_first_mouse_enter = true;

static camera_t camera;

static vec3 light_pos = {-.2f, -1.f, -.3f};

int main(int argc, char const *argv[])
{
  glfwSetErrorCallback(_error_cb);

  if (!glfwInit())
  {
    fprintf(stderr, "GLFW init failed\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "Learning OpenGL", NULL, NULL);
  if (window == NULL)
  {
    fputs("GLFW create window failed\n", stderr);
    return 1;
  }

  glfwSetFramebufferSizeCallback(window, _framebuffer_size_cb);
  glfwSetKeyCallback(window, _key_cb);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, _mouse_cb);
  glfwSetScrollCallback(window, _scroll_cb);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress))
  {
    fputs("GLAD init failed\n", stderr);
    return 1;
  }
  glfwSwapInterval(GLFW_TRUE);

  glEnable(GL_DEPTH_TEST);

  cam_init(
      (vec3){0.f, 0.f, 3.f},
      (vec3){0.f, 1.f, 0.f},
      (vec3){0.f, 0.f, -3.f},
      DEFAULT_YAW,
      DEFAULT_PITCH,
      5.f,
      DEFAULT_SENSE,
      DEFAULT_ZOOM,
      &camera);
  cam_set_constrain_pitch(&camera, true);

  shader_t cube_shader;
  if (!shader_init("resources/shaders/cube.vert", "resources/shaders/cube.frag", &cube_shader))
  {
    return 1;
  }

  shader_t light_cube_shader;
  if (!shader_init("resources/shaders/light.vert", "resources/shaders/light.frag", &light_cube_shader))
  {
    return 1;
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
    return 1;
  }

  GLuint specular_map;
  if (!_create_texture("resources/textures/container2_specular.png", &specular_map))
  {
    return 1;
  }

  shader_use(&cube_shader);
  shader_set_int(&cube_shader, "material.diffuse", 0);
  shader_set_int(&cube_shader, "material.specular", 1);

  while (!glfwWindowShouldClose(window))
  {
    float current_time = glfwGetTime();
    frame_time = current_time - last_frame_time;
    last_frame_time = current_time;

    glClearColor(.1f, .1f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* light_pos[0] = 1.f + sinf(current_time) * 2.f;
    light_pos[2] = 1.f + cosf(current_time) * 2.f; */

    shader_use(&cube_shader);

    shader_set_vec3(&cube_shader, "viewPos", camera.pos);
    shader_set_float(&cube_shader, "material.shininess", 64.f);

    shader_set_vec3(&cube_shader, "directionalLight.direction", (vec3){-.2f, -1.f, -.3f});
    shader_set_vec3(&cube_shader, "directionalLight.ambient", (vec3){.05f, .05f, .05f});
    shader_set_vec3(&cube_shader, "directionalLight.diffuse", (vec3){.4f, .4f, .4f});
    shader_set_vec3(&cube_shader, "directionalLight.specular", (vec3){.5f, .5f, .5f});

    SET_POINT_LIGHT(&cube_shader, 0, POINT_LIGHT_POSITIONS);
    SET_POINT_LIGHT(&cube_shader, 1, POINT_LIGHT_POSITIONS);
    SET_POINT_LIGHT(&cube_shader, 2, POINT_LIGHT_POSITIONS);
    SET_POINT_LIGHT(&cube_shader, 3, POINT_LIGHT_POSITIONS);

    shader_set_vec3(&cube_shader, "spotLight.position", camera.pos);
    shader_set_vec3(&cube_shader, "spotLight.direction", camera.front);
    shader_set_vec3(&cube_shader, "spotLight.ambient", (vec3){0.f, 0.f, 0.f});
    shader_set_vec3(&cube_shader, "spotLight.diffuse", (vec3){1.f, 1.f, 1.f});
    shader_set_vec3(&cube_shader, "spotLight.specular", (vec3){1.f, 1.f, 1.f});
    shader_set_float(&cube_shader, "spotLight.constant", 1.0f);
    shader_set_float(&cube_shader, "spotLight.linear", 0.09f);
    shader_set_float(&cube_shader, "spotLight.quadratic", 0.032f);
    shader_set_float(&cube_shader, "spotLight.cutOff", cosf(glm_rad(12.5f)));
    shader_set_float(&cube_shader, "spotLight.outerCutOff", cosf(glm_rad(15.f)));

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
    for (int i = 0; i < ARRAYSIZE(CUBE_POSITIONS); i++)
    {
      mat4 cube_model = GLM_MAT4_IDENTITY_INIT;
      glm_translate(cube_model, CUBE_POSITIONS[i]);
      float angle = 20.f * i;
      glm_rotate(cube_model, glm_rad(angle), (vec3){1.f, .3f, .5f});

      shader_set_mat4(&cube_shader, "model", cube_model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    shader_use(&light_cube_shader);
    shader_set_mat4(&light_cube_shader, "projection", projection);
    shader_set_mat4(&light_cube_shader, "view", view);

    glBindVertexArray(light_cube_vao);
    for (int i = 0; i < ARRAYSIZE(POINT_LIGHT_POSITIONS); i++)
    {
      mat4 light_cube_model = GLM_MAT4_IDENTITY_INIT;
      glm_translate(light_cube_model, POINT_LIGHT_POSITIONS[i]);
      glm_scale(light_cube_model, (vec3){.2f, .2f, .2f});
      shader_set_mat4(&light_cube_shader, "model", light_cube_model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
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
