#include "shader.h"

#include <stdlib.h>
#include <stdio.h>

#include "fs.h"

static bool _compile_shader(char const *filename, GLenum shader_type, GLuint *shader)
{
  char *shader_source = fs_read_as_text(filename);
  if (shader_source == NULL)
  {
    fputs("Cannot read file\n", stderr);
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
    glGetShaderInfoLog(new_shader, sizeof(log), NULL, log);
    fprintf(stderr, "SHADER COMPILATION ERROR (%u): %s\n", shader_type, log);

    glDeleteShader(new_shader);
    return false;
  }

  *shader = new_shader;
  return true;
}

static bool _compile_program(GLuint vertex, GLuint frag, GLuint *program)
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
    fprintf(stderr, "SHADER LINKING ERROR: %s\n", log);

    glDeleteProgram(new_program);
    return false;
  }

  *program = new_program;
  return true;
}

bool shader_init(char const *vertex_path, char const *frag_path, shader_t *shader)
{
  GLuint vertex, frag;
  if (!_compile_shader(vertex_path, GL_VERTEX_SHADER, &vertex))
  {
    fprintf(stderr, "Cannot compile shader %s\n", frag_path);
    return false;
  }

  if (!_compile_shader(frag_path, GL_FRAGMENT_SHADER, &frag))
  {
    fprintf(stderr, "Cannot compile shader %s\n", frag_path);
    glDeleteShader(vertex);
    return false;
  }

  bool result = _compile_program(vertex, frag, &shader->program_id);
  glDeleteShader(vertex);
  glDeleteShader(frag);
  if (!result)
  {
    fputs("Cannot compile shader program", stderr);
    return false;
  }

  return true;
}

void shader_deinit(shader_t *shader)
{
  glDeleteProgram(shader->program_id);
}

void shader_use(shader_t *shader)
{
  glUseProgram(shader->program_id);
}

void shader_set_int(shader_t *shader, char const *property, int value)
{
  glUniform1i(glGetUniformLocation(shader->program_id, property), value);
}

void shader_set_bool(shader_t *shader, char const *property, bool value)
{
  shader_set_int(shader, property, (int)value);
}

void shader_set_float(shader_t *shader, char const *property, float value)
{
  glUniform1f(glGetUniformLocation(shader->program_id, property), value);
}

void shader_set_vec3(shader_t *shader, char const *property, vec3 vector)
{
  glUniform3fv(glGetUniformLocation(shader->program_id, property), 1, vector);
}

void shader_set_mat4(shader_t *shader, char const *property, mat4 matrix)
{
  glUniformMatrix4fv(glGetUniformLocation(shader->program_id, property), 1, GL_FALSE, (GLfloat const *)matrix);
}
