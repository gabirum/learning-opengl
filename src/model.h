#if !defined(_MODEL_H_)
#define _MODEL_H_

#include <stddef.h>

#include "mesh.h"
#include "shader.h"

typedef struct model
{
  mesh_t *meshes;
  size_t meshes_size;
} model_t;

void model_init(char const *model_path, model_t *model);
void model_deinit(model_t *model);
void model_draw(model_t *model, shader_t *shader);

#endif // _MODEL_H_
