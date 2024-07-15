#if !defined(_SHADER_H_)
#define _SHADER_H_

#include <stdbool.h>

#include <glad/gl.h>
#include <cglm/types.h>

typedef struct shader_s
{
  unsigned int program_id;
} shader_t;

shader_t *shader_new(char const *vertex_path, char const *frag_path);
void shader_destroy(shader_t *shader);

static inline void shader_use(shader_t *shader)
{
  glUseProgram(shader->program_id);
}

static inline void shader_set_int(shader_t *shader, char const *property, int value)
{
  glUniform1i(glGetUniformLocation(shader->program_id, property), value);
}

static inline void shader_set_bool(shader_t *shader, char const *property, bool value)
{
  shader_set_int(shader, property, (int)value);
}

static inline void shader_set_float(shader_t *shader, char const *property, float value)
{
  glUniform1f(glGetUniformLocation(shader->program_id, property), value);
}

static inline void shader_set_mat4(shader_t *shader, char const *property, mat4 matrix)
{
  glUniformMatrix4fv(glGetUniformLocation(shader->program_id, property), 1, GL_FALSE, (GLfloat const *)matrix);
}

#endif // _SHADER_H_
