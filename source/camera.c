#include <stdbool.h>

#include "cglm.h"

#include "camera.h"

const float default_yaw = -90.0f;
const float default_pitch =  -30.0f;
const float default_speed =  7.5f;
const float default_sensitivity =  0.1f;
const float default_fov =  90.0f;

void camera_update_vectors (Camera* camera)
{
	vec3 front = GLM_VEC3_ZERO_INIT;
	
	front[0] = cos (glm_rad (camera->yaw)) * cos (glm_rad (camera->pitch));
	front[1] = sin (glm_rad (camera->pitch));
	front[2] = sin (glm_rad (camera->yaw)) * cos (glm_rad (camera->pitch));
	glm_vec3_normalize (front);
	glm_vec3_copy (front, camera->front);
	
	vec3 camera_vectors = GLM_VEC3_ZERO_INIT;
	glm_vec3_cross (camera->front, camera->world_up, camera_vectors);
	glm_vec3_normalize (camera_vectors);
	glm_vec3_copy (camera_vectors, camera->right);
	glm_vec3_cross (camera->right, camera->front, camera_vectors);
	glm_vec3_copy (camera_vectors, camera->up);
}

void camera_initialize (Camera* camera)
{
	glm_vec3_copy ((vec3) {0.0f, 4.0f, 3.0f}, camera->position);
	glm_vec3_copy ((vec3) {0.0f, 0.0f, -1.0f}, camera->front);
	glm_vec3_copy ((vec3) {0.0f, 1.0f, 0.0f}, camera->up);
	glm_vec3_copy (camera->up, camera->world_up);

	camera->yaw = default_yaw;
	camera->pitch = default_pitch;

	camera->movement_speed = default_speed;
	camera->mouse_sensitivity = default_sensitivity;
	camera->fov = default_fov;

	camera_update_vectors (camera);

	camera->move_forward = false;
	camera->move_backward = false;
	camera->move_left = false;
	camera->move_right = false;
}

void camera_calculate_view_matrix (Camera* camera)
{
	vec3 position_front = GLM_VEC3_ZERO_INIT;

	glm_vec3_add (camera->position, camera->front, position_front);
	glm_lookat (camera->position, position_front, camera->up, camera->view);
}

void camera_calculate_projection_matrix (Camera* camera, float screen_width, float screen_height)
{
	glm_perspective (glm_rad (camera->fov), screen_width / screen_height, 0.1f, 100.0f, camera->projection);
}

void camera_handle_movement (Camera* camera, float delta_time)
{
	float velocity = camera->movement_speed * delta_time;
	vec3 temp = GLM_VEC3_ZERO_INIT; // to hold vector math output

	if (camera->move_forward)
	{
		glm_vec3_scale (camera->front, velocity, temp);
		glm_vec3_add (camera->position, temp, camera->position);
	}
	if (camera->move_backward)
	{
		glm_vec3_scale (camera->front, velocity, temp);
		glm_vec3_sub (camera->position, temp, camera->position);
	}
	if (camera->move_left)
	{
		glm_vec3_scale (camera->right, velocity, temp);
		glm_vec3_sub (camera->position, temp, camera->position);
	}
	if (camera->move_right)
	{
		glm_vec3_scale (camera->right, velocity, temp);
		glm_vec3_add (camera->position, temp, camera->position);
	}
}

void camera_handle_mouse (Camera* camera, float delta_x, float delta_y, bool constrain_pitch)
{
	camera->yaw += delta_x * camera->mouse_sensitivity;
	camera->pitch += -delta_y * camera->mouse_sensitivity; // negative delta, otheriwse inverted pitch

	if (constrain_pitch)
	{
		camera->pitch = glm_clamp (camera->pitch, -89.0f, 89.0f);
	}

	camera_update_vectors (camera);
}
