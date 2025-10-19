#if !defined(_MESH_H_)
#define _MESH_H_

#include <stddef.h>

#include <glad/gl.h>
#include <cglm/types.h>

#include "shader.h"

#define MAX_BONE_INFLUENCE 4

typedef struct vertex
{
  vec3 position, normal, tangent, bitangent;
  vec2 tex_coords;
  int bone_ids[MAX_BONE_INFLUENCE];
  float weights[MAX_BONE_INFLUENCE];
} vertex_t;

enum texture_type
{
  TEXTURE_DIFFUSE,
  TEXTURE_SPECULAR,
  TEXTURE_NORMAL,
  TEXTURE_HEIGHT
};

typedef struct texture
{
  GLuint id;
  enum texture_type type;
  char *path;
} texture_t;

typedef struct mesh
{
  vertex_t *vertices;
  GLuint *indices;
  texture_t *textures;
  size_t vertices_size, indices_size, textures_size;
  GLuint vao, vbo, ebo;
} mesh_t;

void mesh_init(
    vertex_t *vertices,
    size_t vertices_size,
    GLuint *indices,
    size_t indices_size,
    texture_t *textures,
    size_t textures_size,
    mesh_t *mesh);
void mesh_deinit(mesh_t *mesh);
void mesh_draw(mesh_t *mesh, shader_t *shader);

#endif // _MESH_H_
