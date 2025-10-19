#include "mesh.h"

#include <stdio.h>

static void _setup_mesh(mesh_t *mesh)
{
  glGenVertexArrays(1, &mesh->vao);
  glGenBuffers(1, &mesh->vbo);
  glGenBuffers(1, &mesh->ebo);

  glBindVertexArray(mesh->vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

  glBufferData(GL_ARRAY_BUFFER, mesh->vertices_size * sizeof(vertex_t), mesh->vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_size * sizeof(GLuint), mesh->indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tangent));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, bitangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, tex_coords));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 3, GL_INT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, bone_ids));

  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *)offsetof(vertex_t, weights));

  glBindVertexArray(0);
}

void mesh_init(
    vertex_t *vertices,
    size_t vertices_size,
    GLuint *indices,
    size_t indices_size,
    texture_t *textures,
    size_t textures_size,
    mesh_t *mesh)
{
  mesh->vertices = vertices;
  mesh->vertices_size = vertices_size;
  mesh->indices = indices;
  mesh->indices_size = indices_size;
  mesh->textures = textures;
  mesh->textures_size = textures_size;

  _setup_mesh(mesh);
}

void mesh_deinit(mesh_t *mesh)
{
  if (mesh == NULL)
  {
    return;
  }

  glDeleteBuffers(1, &mesh->ebo);
  glDeleteBuffers(1, &mesh->vbo);
  glDeleteVertexArrays(1, &mesh->vao);
}

void mesh_draw(mesh_t *mesh, shader_t *shader)
{
  GLuint diffuse_index = 0;
  GLuint specular_index = 0;
  GLuint normal_index = 0;
  GLuint height_index = 0;

  for (size_t i = 0; i < mesh->textures_size; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);

    char const *name;
    switch (mesh->textures[i].type)
    {
    case TEXTURE_DIFFUSE:
      diffuse_index++;
      name = "texture_diffuse";
      break;
    case TEXTURE_SPECULAR:
      specular_index++;
      name = "texture_specular";
      break;
    case TEXTURE_NORMAL:
      normal_index++;
      name = "texture_normal";
      break;
    case TEXTURE_HEIGHT:
      height_index++;
      name = "texture_height";
      break;
    default:
      name = "unknown";
      break;
    }

    char property_name[100];
    snprintf(property_name, sizeof(property_name), "material.%s%zu", name, i);
    shader_set_float(shader, property_name, (float)i);
    glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
  }

  glBindVertexArray(mesh->vao);
  glDrawElements(GL_TRIANGLES, mesh->indices_size, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0);
}
