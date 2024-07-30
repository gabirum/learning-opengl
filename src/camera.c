#include "camera.h"

#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>

inline void cam_init(vec3 pos, vec3 up, vec3 front, float yaw, float pitch, float mov_speed, float sense, float zoom, camera_t *camera)
{
  memcpy(camera->pos, pos, sizeof(vec3));
  memcpy(camera->up, up, sizeof(vec3));
  memcpy(camera->front, front, sizeof(vec3));
  camera->yaw = yaw;
  camera->pitch = pitch;
  camera->mov_speed = mov_speed;
  camera->mouse_sense = sense;
  camera->zoom = zoom;
}

inline void cam_init_defaults(camera_t *camera)
{
  cam_init(DEFAULT_POS, DEFAULT_UP, DEFAULT_FRONT, DEFAULT_YAW, DEFAULT_PITCH, DEFAULT_SPEED, DEFAULT_SENSE, DEFAULT_ZOOM, camera);
}

inline void cam_get_pos(camera_t *camera, vec3 pos)
{
  memcpy(pos, camera->pos, sizeof(vec3));
}

inline bool cam_get_constrain_pitch(camera_t *camera)
{
  return camera->constrain_pitch;
}

inline void cam_set_constrain_pitch(camera_t *camera, bool value)
{
  camera->constrain_pitch = value;
}

inline float cam_get_zoom(camera_t *camera)
{
  return camera->zoom;
}

inline void cam_get_view_matrix(camera_t *camera, mat4 view_matrix)
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

  case CAMERA_UP:
  {
    glm_vec3_scale(camera->up, velocity, scale);
    glm_vec3_add(camera->pos, scale, result);
    memcpy(camera->pos, result, sizeof(vec3));
    break;
  }

  case CAMERA_DOWN:
  {
    glm_vec3_scale(camera->up, velocity, scale);
    glm_vec3_sub(camera->pos, scale, result);
    memcpy(camera->pos, result, sizeof(vec3));
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

inline void cam_process_scroll(camera_t *camera, float offset)
{
  camera->zoom = glm_clamp(camera->zoom + offset, 1.f, 70.f);
}
