#ifndef _camera_h
#define _camera_h

#include <stdbool.h>

#include "cglm.h"

typedef struct camera_s
{
	mat4 view;
	mat4 projection;

	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 world_up;

	float yaw;
	float pitch;

	float movement_speed;
	float mouse_sensitivity;
	float fov;

	bool move_forward;
	bool move_backward;
	bool move_left;
	bool move_right;
} Camera;

void camera_update_vectors (Camera* camera);
void camera_initialize (Camera* camera);
void camera_calculate_view_matrix (Camera* camera);
void camera_calculate_projection_matrix (Camera* camera, float screen_width, float screen_height);
void camera_handle_movement (Camera* camera, float delta_time);
void camera_handle_mouse (Camera* camera, float delta_x, float delta_y, bool constrain_pitch);

#endif
