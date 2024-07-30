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
extern inline void shader_deinit(shader_t *shader);

extern inline void shader_use(shader_t *shader);

extern inline void shader_set_int(shader_t *shader, char const *property, int value);
extern inline void shader_set_bool(shader_t *shader, char const *property, bool value);
extern inline void shader_set_float(shader_t *shader, char const *property, float value);
extern inline void shader_set_vec3(shader_t *shader, char const *property, vec3 vector);
extern inline void shader_set_mat4(shader_t *shader, char const *property, mat4 matrix);

#endif // _SHADER_H_
