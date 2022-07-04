// cglm uses this on windows, and nuklear apparently needs it for something (from sokol_nuklear)
#define _CRT_SECURE_NO_WARNINGS (1)

#include <stdio.h>
#include <math.h>

#include "implementations.h"
#include "sokol.h"
#include "nuklear.h"
#include "sokol_nuklear.h"
#include "cglm.h"

#include "grid_glsl.h"
#include "camera.h"

static struct
{
	uint64_t last_frame_time;
	uint64_t frame_time;

	Camera camera;

	bool first_mouse;
	float mouse_x;
	float mouse_y;
	float last_mouse_x;
	float last_mouse_y;
	
	sg_pipeline cube_pipeline;
	sg_bindings bindings;
	sg_pass_action pass_action;
	vec3 object_color;
	vec3 light_color;
	vec3 light_position;

	sg_pipeline grid_pipeline;
	sg_bindings grid_bindings;

	bool camera_locked;
} state;

void initialize (void)
{
	state.first_mouse = true;
	state.mouse_x = 0.0f;
	state.mouse_y = 0.0f;
	state.last_mouse_x = 0.0f;
	state.last_mouse_y = 0.0f;
	state.camera_locked = true;

	camera_initialize (&state.camera);

	sg_setup
	(
		&(sg_desc)
		{
			.context = sapp_sgcontext ()
		}
	);

	stm_setup ();

	sfetch_setup
	(
		&(sfetch_desc_t)
		{
			.max_requests = 8,
			.num_channels = 1,
			.num_lanes = 1
		}
	);

	snk_setup
	(
		&(snk_desc_t)
		{
			.dpi_scale = sapp_dpi_scale ()
		}
	);

	glm_vec3_copy ((vec3) {1.0f, 0.5f, 0.31f}, state.object_color);
	glm_vec3_copy ((vec3) {1.0f, 1.0f, 1.0f}, state.light_color);
	glm_vec3_copy ((vec3) {1.2f, 1.0f, 2.0f}, state.light_position);

	float voxel_vertices[] = {
		// positions          // normal vectors
		-0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
		-0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f, 
		
		-0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.0f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		
		-0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,
		
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	state.bindings.vertex_buffers[0] = sg_make_buffer
	(
		&(sg_buffer_desc)
		{
			.size = sizeof (voxel_vertices),
			.data = SG_RANGE (voxel_vertices),
			.label = "cube_vertices"
		}
	);

	sg_shader cube_shader = sg_make_shader (cube_shader_desc (sg_query_backend ()));

	// create a pipeline object (default render states are fine)
	state.cube_pipeline = sg_make_pipeline
	(
		&(sg_pipeline_desc)
		{
			.shader = cube_shader,
			.layout =
			{
				.attrs = 
				{
					[ATTR_vs_cube_position].format = SG_VERTEXFORMAT_FLOAT3,
					[ATTR_vs_cube_normal_in].format = SG_VERTEXFORMAT_FLOAT3
				}
			},
			.depth =
			{
				.compare = SG_COMPAREFUNC_LESS_EQUAL,
				.write_enabled = true
			},
			.label = "cube-pipeline"
		}
	);

	float grid_vertices[] =
	{
		// positions
		 1.0f,  1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f, 
		-1.0f,  1.0f,  0.0f, 

		-1.0f, -1.0f,  0.0f, 
		 1.0f,  1.0f,  0.0f, 
		 1.0f, -1.0f,  0.0f, 
	};

	state.grid_bindings.vertex_buffers[0] = sg_make_buffer
	(
		&(sg_buffer_desc)
		{
			.size = sizeof (grid_vertices),
			.data = SG_RANGE (grid_vertices),
			.label = "grid_vertices"
		}
	);

	sg_shader grid_shader = sg_make_shader (grid_shader_desc (sg_query_backend ()));

	// create a pipeline object (default render states are fine)
	state.grid_pipeline = sg_make_pipeline
	(
		&(sg_pipeline_desc)
		{
			.shader = grid_shader,
			.layout =
			{
				.attrs = 
				{
					[ATTR_vs_grid_position].format = SG_VERTEXFORMAT_FLOAT3,
				}
			},
			.depth =
			{
				.compare = SG_COMPAREFUNC_LESS,
				.write_enabled = false
			},
			.colors[0].blend =
			{
				.enabled = true,
				.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
				.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.op_rgb = SG_BLENDOP_ADD,
				.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
				.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.op_alpha = SG_BLENDOP_ADD
			},
			.label = "grid-pipeline"
		}
	);

	// a pass action which clears the framebuffer
	state.pass_action = (sg_pass_action) {
		.colors[0] = {.action = SG_ACTION_CLEAR, .value = {0.42f, 0.82f, 1.0f, 1.0f}}
	};
}

static int draw_ui (struct nk_context* context);

void frame (void)
{
	sfetch_dowork ();
	state.frame_time = stm_laptime (&state.last_frame_time);

	if (!state.camera_locked)
	{
		camera_handle_movement (&state.camera, (float) stm_sec (state.frame_time));
	}

	struct nk_context* context = snk_new_frame ();
	draw_ui (context);

	camera_calculate_view_matrix (&state.camera);

	camera_calculate_projection_matrix (&state.camera, sapp_width (), sapp_height ());

	sg_begin_default_pass (&state.pass_action, sapp_width (), sapp_height ());
	sg_apply_pipeline (state.cube_pipeline);
	sg_apply_bindings (&state.bindings);

	vs_cube_params_t vs_cube_params;
	glm_mat4_copy (state.camera.view, vs_cube_params.view);
	glm_mat4_copy (state.camera.projection, vs_cube_params.projection);
	glm_mat4_copy (GLM_MAT4_IDENTITY, vs_cube_params.model);

	sg_apply_uniforms (SG_SHADERSTAGE_VS, SLOT_vs_cube_params, &SG_RANGE (vs_cube_params));

	fs_cube_params_t fs_cube_params;
	glm_vec3_copy ((vec3) {0.8f, 0.0f, 0.2f}, fs_cube_params.object_color);
	glm_vec3_copy ((vec3) {1.0f, 1.0f, 1.0f}, fs_cube_params.light_color);
	glm_vec3_copy ((vec3) {2.0f, 2.0f, 2.0f}, fs_cube_params.light_position);
	glm_vec3_copy (state.camera.position, fs_cube_params.view_position);

	sg_apply_uniforms (SG_SHADERSTAGE_FS, SLOT_fs_cube_params, &SG_RANGE (fs_cube_params));

	sg_draw (0, 36, 1);

	glm_translate (vs_cube_params.model, (vec3) {0.0f, 1.0f, 0.0f});
	sg_apply_uniforms (SG_SHADERSTAGE_VS, SLOT_vs_cube_params, &SG_RANGE (vs_cube_params));
	sg_apply_uniforms (SG_SHADERSTAGE_FS, SLOT_fs_cube_params, &SG_RANGE (fs_cube_params));

	sg_draw (0, 36, 1);

	// draw the grid
	sg_apply_pipeline (state.grid_pipeline);
	sg_apply_bindings (&state.grid_bindings);

	vs_grid_params_t vs_grid_params;
	glm_mat4_copy (state.camera.view, vs_grid_params.view);
	glm_mat4_copy (state.camera.projection, vs_grid_params.projection);
	glm_mat4_copy (GLM_MAT4_IDENTITY, vs_grid_params.model);
	glm_vec3_copy (state.camera.position, vs_grid_params.camera_position);

	sg_apply_uniforms (SG_SHADERSTAGE_VS, SLOT_vs_grid_params, &SG_RANGE (vs_grid_params));

	sg_draw (0, 6, 1);


	snk_render (sapp_width (), sapp_height ());

	sg_end_pass ();
	sg_commit ();
}

void cleanup (void)
{
	sg_destroy_pipeline (state.cube_pipeline);
	sg_destroy_pipeline (state.grid_pipeline);
	snk_shutdown ();
	sg_shutdown ();
	sfetch_shutdown ();
}

void event_handle (const sapp_event* event)
{
	snk_handle_event (event);

	switch (event->type)
	{
		case SAPP_EVENTTYPE_KEY_DOWN:
			switch (event->key_code)
			{
				case SAPP_KEYCODE_ESCAPE:
					sapp_request_quit ();
					break;
				case SAPP_KEYCODE_W:
					state.camera.move_forward = true;
					break;
				case SAPP_KEYCODE_S:
					state.camera.move_backward = true;
					break;
				case SAPP_KEYCODE_A:
					state.camera.move_left = true;
					break;
				case SAPP_KEYCODE_D:
					state.camera.move_right = true;
					break;
				case SAPP_KEYCODE_SPACE:
					state.camera_locked = !state.camera_locked;
					sapp_lock_mouse (!state.camera_locked);
					break;
				default:
					break;
			}
			break;
		case SAPP_EVENTTYPE_KEY_UP:
			switch (event->key_code)
			{
				case SAPP_KEYCODE_W:
					state.camera.move_forward = false;
					break;
				case SAPP_KEYCODE_S:
					state.camera.move_backward = false;
					break;
				case SAPP_KEYCODE_A:
					state.camera.move_left = false;
					break;
				case SAPP_KEYCODE_D:
					state.camera.move_right = false;
					break;
				default:
					break;
			}
			break;
		case SAPP_EVENTTYPE_MOUSE_MOVE:
		{
			if (!state.camera_locked)
			{
				camera_handle_mouse (&state.camera, event->mouse_dx, event->mouse_dy, true);
			}
			break;
		}
		default:
			break;
	}
}

sapp_desc sokol_main (int argc, char* argv[])
{
	return (sapp_desc)
	{
		.init_cb = initialize,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.event_cb = event_handle,
		.width = 1600,
		.height = 900,
		.window_title = "grid test"
	};
}

static int draw_ui (struct nk_context* context)
{
	context->style.window.header.align = NK_HEADER_RIGHT;

	if (nk_begin (context, "grid ui", nk_rect (0, 0, 300, 300), NK_WINDOW_TITLE))
	{
		nk_layout_row_static (context, 20, 280, 1);
	}

	nk_end (context);

	return !nk_window_is_closed (context, "Test UI");
}
