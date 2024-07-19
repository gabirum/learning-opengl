#if !defined(_CAMERA_H_)
#define _CAMERA_H_

#include <stdbool.h>

#include <cglm/cglm.h>

#define DEFAULT_YAW -90.f
#define DEFAULT_PITCH 0.f
#define DEFAULT_SPEED 2.5f
#define DEFAULT_SENSE .1f
#define DEFAULT_ZOOM 45.f
#define DEFAULT_POS ((vec3){0.f, 0.f, 0.f})
#define DEFAULT_UP ((vec3){0.f, 1.f, 0.f})
#define DEFAULT_FRONT ((vec3){0.f, 0.f, -1.f})

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

camera_t *cam_new(vec3 pos, vec3 up, vec3 front, float yaw, float pitch, float mov_speed, float sense, float zoom);

static inline camera_t *cam_new_defaults()
{
  return cam_new(DEFAULT_POS, DEFAULT_UP, DEFAULT_FRONT, DEFAULT_YAW, DEFAULT_PITCH, DEFAULT_SPEED, DEFAULT_SENSE, DEFAULT_ZOOM);
}

void cam_destroy(camera_t *camera);

static inline bool cam_get_constrain_pitch(camera_t *camera)
{
  return camera->constrain_pitch;
}

static inline void cam_set_constrain_pitch(camera_t *camera, bool value)
{
  camera->constrain_pitch = value;
}

static inline float cam_get_zoom(camera_t *camera)
{
  return camera->zoom;
}

void cam_get_view_matrix(camera_t *camera, mat4 view_matrix);

void cam_process_key(camera_t *camera, enum camera_mov_e direction, float frame_time);
void cam_process_mouse(camera_t *camera, float xoff, float yoff);
void cam_process_scroll(camera_t *camera, float offset);

#endif // _CAMERA_H_
