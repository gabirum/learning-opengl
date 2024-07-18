#include "camera.h"

#include <stdlib.h>
#include <string.h>

camera_t *cam_new(vec3 pos, vec3 up, vec3 front, float yaw, float pitch, float mov_speed, float sense, float zoom)
{
  camera_t *camera = calloc(1, sizeof(camera_t));
  if (camera == NULL)
  {
    return NULL;
  }

  memcpy(camera->pos, pos, sizeof(vec3));
  memcpy(camera->up, up, sizeof(vec3));
  memcpy(camera->front, front, sizeof(vec3));
  camera->yaw = yaw;
  camera->pitch = pitch;
  camera->mov_speed = mov_speed;
  camera->mouse_sense = sense;
  camera->zoom = zoom;

  return camera;
}

void cam_destroy(camera_t *camera)
{
  free(camera);
}

void cam_get_view_matrix(camera_t *camera, mat4 view_matrix)
{
  vec3 center;
  glm_vec3_add(camera->pos, camera->front, center);
  glm_lookat(camera->pos, center, camera->up, view_matrix);
}

void cam_process_key(camera_t *camera, enum camera_mov_e direction, float frame_time)
{
  float velocity = camera->mov_speed * frame_time;

  vec3 cross, scale, result;
  switch (direction)
  {
  case CAMERA_FORWARD:
  {
    glm_vec3_scale(camera->front, velocity, scale);
    glm_vec3_add(camera->pos, scale, result);
    memcpy(camera->pos, result, sizeof(vec3));
    break;
  }

  case CAMERA_BACKWARD:
  {
    glm_vec3_scale(camera->front, velocity, scale);
    glm_vec3_sub(camera->pos, scale, result);
    memcpy(camera->pos, result, sizeof(vec3));
    break;
  }

  case CAMERA_LEFT:
  {
    glm_cross(camera->front, camera->up, cross);
    glm_normalize(cross);
    glm_vec3_scale(cross, velocity, scale);
    glm_vec3_sub(camera->pos, scale, result);
    memcpy(camera->pos, result, sizeof(vec3));
    break;
  }

  case CAMERA_RIGHT:
  {
    glm_cross(camera->front, camera->up, cross);
    glm_normalize(cross);
    glm_vec3_scale(cross, velocity, scale);
    glm_vec3_add(camera->pos, scale, result);
    memcpy(camera->pos, result, sizeof(vec3));
    break;
  }

  default:
    break;
  }
}

void cam_process_mouse(camera_t *camera, float xoff, float yoff)
{
  xoff *= camera->mouse_sense;
  yoff *= camera->mouse_sense;

  camera->yaw += xoff;
  camera->pitch += yoff;

  if (camera->constrain_pitch)
  {
    camera->pitch = glm_clamp(camera->pitch, -89.f, 89.f);
  }

  vec3 direction;
  direction[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
  direction[1] = sinf(glm_rad(camera->pitch));
  direction[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
  glm_normalize(direction);
  memcpy(camera->front, direction, sizeof(direction));
}

void cam_process_scroll(camera_t *camera, float offset)
{
  camera->zoom = glm_clamp(camera->zoom + offset, 1.f, 70.f);
}
