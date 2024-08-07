#include "shader.h"

#include <stdlib.h>
#include <stdio.h>

#include "fs.h"

#define LOG_ERR(format, ...) fprintf(stderr, "error @ %s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

static bool compile_shader(char const *filename, GLenum shader_type, GLuint *shader)
{
  uint8_t *shader_source = fs_read_all_file(filename);
  if (shader_source == NULL)
  {
    LOG_ERR("cannot open file: %s", filename);
    return false;
  }

  GLuint new_shader = glCreateShader(shader_type);
  glShaderSource(new_shader, 1, (char const *const *)&shader_source, NULL);
  glCompileShader(new_shader);
  free(shader_source);

  int success;
  glGetShaderiv(new_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    char log[512];
    glGetShaderInfoLog(new_shader, 512, NULL, log);
    LOG_ERR("SHADER COMPILATION ERROR (%u): %s", shader_type, log);

    glDeleteShader(new_shader);
    return false;
  }

  *shader = new_shader;
  return true;
}

static bool compile_program(GLuint vertex, GLuint frag, GLuint *program)
{
  GLuint new_program = glCreateProgram();
  glAttachShader(new_program, vertex);
  glAttachShader(new_program, frag);
  glLinkProgram(new_program);

  int success;
  glGetProgramiv(new_program, GL_LINK_STATUS, &success);
  if (!success)
  {
    char log[512];
    glGetProgramInfoLog(new_program, 512, NULL, log);
    LOG_ERR("SHADER LINKING ERROR: %s", log);

    glDeleteProgram(new_program);
    return false;
  }

  *program = new_program;
  return true;
}

bool shader_init(char const *vertex_path, char const *frag_path, shader_t *shader)
{
  GLuint vertex, frag;
  if (!compile_shader(vertex_path, GL_VERTEX_SHADER, &vertex))
  {
    return false;
  }

  if (!compile_shader(frag_path, GL_FRAGMENT_SHADER, &frag))
  {
    glDeleteShader(vertex);
    return false;
  }

  bool result = compile_program(vertex, frag, &shader->program_id);
  glDeleteShader(vertex);
  glDeleteShader(frag);
  if (!result)
  {
    return false;
  }

  return true;
}

extern inline void shader_deinit(shader_t *shader);
extern inline void shader_use(shader_t *shader);
extern inline void shader_set_int(shader_t *shader, char const *property, int value);
extern inline void shader_set_bool(shader_t *shader, char const *property, bool value);
extern inline void shader_set_float(shader_t *shader, char const *property, float value);
extern inline void shader_set_vec3(shader_t *shader, char const *property, vec3 vector);
extern inline void shader_set_mat4(shader_t *shader, char const *property, mat4 matrix);
