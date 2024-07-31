#if !defined(_SHADER_H_)
#define _SHADER_H_

#include <stdbool.h>

#include <glad/gl.h>
#include <cglm/types.h>

typedef struct shader_s
{
  GLuint program_id;
} shader_t;

bool shader_init(char const *vertex_path, char const *frag_path, shader_t *shader);

inline void shader_deinit(shader_t *shader)
{
  glDeleteProgram(shader->program_id);
}

inline void shader_use(shader_t *shader)
{
  glUseProgram(shader->program_id);
}

inline void shader_set_int(shader_t *shader, char const *property, int value)
{
  glUniform1i(glGetUniformLocation(shader->program_id, property), value);
}

inline void shader_set_bool(shader_t *shader, char const *property, bool value)
{
  shader_set_int(shader, property, (int)value);
}

inline void shader_set_float(shader_t *shader, char const *property, float value)
{
  glUniform1f(glGetUniformLocation(shader->program_id, property), value);
}

inline void shader_set_vec3(shader_t *shader, char const *property, vec3 vector)
{
  glUniform3fv(glGetUniformLocation(shader->program_id, property), 1, vector);
}

inline void shader_set_mat4(shader_t *shader, char const *property, mat4 matrix)
{
  glUniformMatrix4fv(glGetUniformLocation(shader->program_id, property), 1, GL_FALSE, (GLfloat const *)matrix);
}

#endif // _SHADER_H_
