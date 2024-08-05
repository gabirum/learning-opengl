#if !defined(_CAMERA_H_)
#define _CAMERA_H_

#include <stdbool.h>
#include <string.h>

#include <cglm/cglm.h>

#define DEFAULT_YAW -90.f
#define DEFAULT_PITCH 0.f
#define DEFAULT_SPEED 2.5f
#define DEFAULT_SENSE .1f
#define DEFAULT_ZOOM 45.f
#define DEFAULT_POS ((vec3){0.f, 0.f, 0.f})
#define DEFAULT_UP ((vec3){0.f, 0.f, 0.f})
#define DEFAULT_FRONT ((vec3){0.f, 0.f, 0.f})

enum camera_mov_e
{
  CAMERA_FORWARD,
  CAMERA_BACKWARD,
  CAMERA_LEFT,
  CAMERA_RIGHT,
  CAMERA_UP,
  CAMERA_DOWN,
};

typedef struct camera_s
{
  bool constrain_pitch;
  vec3 pos, front, up, right, world;
  float yaw, pitch, mov_speed, mouse_sense, zoom;
} camera_t;

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

// static inline due to glm functions are static inline
static inline void cam_get_view_matrix(camera_t *camera, mat4 view_matrix)
{
  vec3 center;
  glm_vec3_add(camera->pos, camera->front, center);
  glm_lookat(camera->pos, center, camera->up, view_matrix);
}

// static inline due to glm functions are static inline
static inline void cam_process_scroll(camera_t *camera, float offset)
{
  camera->zoom = glm_clamp(camera->zoom + offset, 1.f, 70.f);
}

void cam_process_key(camera_t *camera, enum camera_mov_e direction, float frame_time);
void cam_process_mouse(camera_t *camera, float xoff, float yoff);

#endif // _CAMERA_H_
