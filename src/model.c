#include "model.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#define COPY_VEC2(dest, src) \
  do                         \
  {                          \
    dest[0] = src.x;         \
    dest[1] = src.y;         \
  } while (0)

#define COPY_VEC3(dest, src) \
  do                         \
  {                          \
    dest[0] = src.x;         \
    dest[1] = src.y;         \
    dest[2] = src.z;         \
  } while (0)

static struct
{
  texture_t *textures;
  size_t count, size;
} loaded_textures = {0};

static void _add_loaded_texture(texture_t const *texture)
{
  if (loaded_textures.textures == NULL)
  {
    texture_t *textures = malloc(10 * sizeof(texture_t));
    assert(textures != NULL);
    loaded_textures.textures = textures;
    loaded_textures.size = 10;
    loaded_textures.count = 0;
  }

  if (loaded_textures.count >= loaded_textures.size)
  {
    size_t new_size = loaded_textures.size + (loaded_textures.size >> 1);
    texture_t *new_textures_block = realloc(loaded_textures.textures, new_size * sizeof(texture_t));
    assert(new_textures_block != NULL);
    loaded_textures.textures = new_textures_block;
    loaded_textures.size = new_size;
  }

  memcpy(&loaded_textures.textures[loaded_textures.count++], texture, sizeof(texture_t));
}

static bool _texture_from_file(char const *path, GLuint *texture)
{
  int width, height, components;
  unsigned char *data = stbi_load(path, &width, &height, &components, 0);
  assert(data != NULL);

  GLenum format;
  switch (components)
  {
  case 1:
    format = GL_RED;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;

  default:
    return false;
  }

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);

  *texture = texture_id;
  return true;
}

static void _load_material_textures(
    struct aiMaterial const *material,
    enum aiTextureType assimp_type,
    enum texture_type type,
    unsigned int texture_count,
    texture_t *textures)
{
  for (unsigned int i = 0; i < texture_count; i++)
  {
    struct aiString str;
    aiGetMaterialTexture(material, assimp_type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

    bool skip = false;
    for (size_t j = 0; j < loaded_textures.count; i++)
    {
      if (strcmp(loaded_textures.textures[j].path, str.data) == 0)
      {
        memcpy(&textures[i], &loaded_textures.textures[j], sizeof(texture_t));
        skip = true;
        break;
      }
    }

    if (!skip)
    {
      texture_t *texture = &textures[i];
      assert(_texture_from_file(str.data, &texture->id));
      texture->type = type;
      texture->path = strdup(str.data);
      assert(texture->path != NULL);

      _add_loaded_texture(texture);
    }
  }
}

static void _process_mesh(struct aiMesh *mesh, struct aiScene const *scene, mesh_t *output_mesh)
{
  vertex_t *vertices = calloc(mesh->mNumVertices, sizeof(vertex_t));
  assert(vertices != NULL);
  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    vertex_t *vertex = &vertices[i];
    COPY_VEC3(vertex->position, mesh->mVertices[i]);

    if (mesh->mNormals != NULL)
    {
      COPY_VEC3(vertex->normal, mesh->mNormals[i]);
    }

    if (mesh->mTextureCoords[0])
    {
      COPY_VEC2(vertex->tex_coords, mesh->mTextureCoords[0][i]);
      COPY_VEC3(vertex->tangent, mesh->mTangents[i]);
      COPY_VEC3(vertex->bitangent, mesh->mBitangents[i]);
    }
  }

  size_t indices_size = 0;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    indices_size += mesh->mFaces[i].mNumIndices;
  }

  unsigned int *indices = calloc(indices_size, sizeof(unsigned int));
  assert(indices != NULL);
  size_t indices_offset = 0;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    struct aiFace face = mesh->mFaces[i];
    memcpy(&indices[indices_offset], face.mIndices, face.mNumIndices * sizeof(unsigned int));
    indices_offset += face.mNumIndices;
  }

  struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  unsigned int diffuse_count = aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);
  unsigned int specular_count = aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);
  unsigned int normal_count = aiGetMaterialTextureCount(material, aiTextureType_HEIGHT);
  unsigned int height_count = aiGetMaterialTextureCount(material, aiTextureType_AMBIENT);
  size_t textures_size = diffuse_count + specular_count + normal_count + height_count;
  texture_t *textures = calloc(textures_size, sizeof(texture_t));
  assert(textures != NULL);

  texture_t *tmp = textures;
  _load_material_textures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE, diffuse_count, tmp);
  tmp += diffuse_count;
  _load_material_textures(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR, specular_count, tmp);
  tmp += specular_count;
  _load_material_textures(material, aiTextureType_HEIGHT, TEXTURE_NORMAL, normal_count, tmp);
  tmp += normal_count;
  _load_material_textures(material, aiTextureType_AMBIENT, TEXTURE_HEIGHT, height_count, tmp);

  mesh_init(vertices, mesh->mNumVertices, indices, indices_size, textures, textures_size, output_mesh);
}

static size_t _count_meshes(struct aiNode *node)
{
  size_t meshes_count = node->mNumMeshes;

  for (unsigned int i = 0; node->mNumChildren; i++)
  {
    meshes_count += _count_meshes(node->mChildren[i]);
  }

  return meshes_count;
}

static void _process_node(struct aiNode *node, struct aiScene const *scene, mesh_t *meshes, size_t *index)
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    _process_mesh(mesh, scene, &meshes[(*index)++]);
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    _process_node(node->mChildren[i], scene, meshes, index);
  }
}

void model_init(char const *model_path, model_t *model)
{
  struct aiScene const *scene = aiImportFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    fprintf(stderr, "ASSIMP error: %s\n", aiGetErrorString());
    exit(1);
  }

  size_t meshes_size = _count_meshes(scene->mRootNode);
  mesh_t *meshes = calloc(meshes_size, sizeof(mesh_t));
  assert(meshes != NULL);
  size_t mesh_index = 0;
  _process_node(scene->mRootNode, scene, meshes, &mesh_index);
  model->meshes = meshes;
  model->meshes_size = meshes_size;
}

void model_deinit(model_t *model)
{
  if (model == NULL)
  {
    return;
  }

  for (size_t i = 0; i < model->meshes_size; i++)
  {
    mesh_t *mesh = &model->meshes[i];
    mesh_deinit(mesh);
    free(mesh->textures);
    free(mesh->indices);
    free(mesh->vertices);
  }

  free(model->meshes);
}

void model_draw(model_t *model, shader_t *shader)
{
  for (size_t i = 0; i < model->meshes_size; i++)
  {
    mesh_draw(&model->meshes[i], shader);
  }
}
