#include "shader.h"

#include <stdlib.h>
#include <stdio.h>

#include "fs.h"

#define LOG_ERR(format, ...) fprintf(stderr, "err in file %s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

static bool compile_shader(char const *filename, GLenum shader_type, unsigned int *shader)
{
  uint8_t *shader_source = fs_read_all_file(filename);
  if (shader_source == NULL)
    return false;

  unsigned int new_shader = glCreateShader(shader_type);
  glShaderSource(new_shader, 1, (char const *const *)&shader_source, NULL);
  glCompileShader(new_shader);
  free(shader_source);

  int success;
  glGetShaderiv(new_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    char log[512];
    glGetShaderInfoLog(new_shader, 512, NULL, log);
    LOG_ERR("SHADER COMPILATION ERROR (%u): %s\n", shader_type, log);

    glDeleteShader(new_shader);
    return false;
  }

  *shader = new_shader;
  return true;
}

static bool compile_program(unsigned int vertex, unsigned int frag, unsigned int *program)
{
  unsigned int new_program = glCreateProgram();
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

shader_t *shader_new(char const *vertex_path, char const *frag_path)
{
  shader_t *shader = malloc(sizeof(shader_t));
  if (shader == NULL)
  {
    LOG_ERR("Allocation failed");
    return NULL;
  }

  unsigned int vertex, frag;
  if (!compile_shader(vertex_path, GL_VERTEX_SHADER, &vertex))
  {
    free(shader);
    return NULL;
  }

  if (!compile_shader(frag_path, GL_FRAGMENT_SHADER, &frag))
  {
    free(shader);
    glDeleteShader(vertex);
    return NULL;
  }

  bool result = compile_program(vertex, frag, &shader->program_id);
  glDeleteShader(vertex);
  glDeleteShader(frag);
  if (!result)
  {
    free(shader);
    return NULL;
  }

  return shader;
}

void shader_destroy(shader_t *shader)
{
  glDeleteProgram(shader->program_id);
  free(shader);
}
