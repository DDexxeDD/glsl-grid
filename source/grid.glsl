@ctype vec3 vec3
@ctype mat4 mat4


@vs vs_cube
in vec3 position;
in vec3 normal_in;

out vec3 frag_position;
out vec3 normal;

uniform vs_cube_params
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

void main ()
{
	gl_Position = projection * view * model * vec4 (position, 1.0);
	frag_position = vec3 (model * vec4 (position, 1.0));
	normal = mat3 (transpose (inverse (model))) * normal_in;
}
@end


@fs fs_cube
in vec3 frag_position;
in vec3 normal;

out vec4 frag_color;

uniform fs_cube_params
{
	vec3 object_color;
	vec3 light_color;
	vec3 light_position;
	vec3 view_position;
};

void main ()
{
	float ambient_strength = 0.1;
	vec3 ambient = ambient_strength * light_color;

	vec3 norm = normalize (normal);
	vec3 light_direction = normalize (light_position - frag_position);
	float diff = max (dot (norm, light_direction), 0.0);
	vec3 diffuse = diff * light_color;

	float specular_strength = 0.5;
	vec3 view_direction = normalize (view_position - frag_position);
	vec3 reflect_direction = reflect (-light_direction, norm);

	float spec = pow (max (dot (view_direction, reflect_direction), 0.0), 32);
	vec3 specular = specular_strength * spec * light_color;

	vec3 result = (ambient + diffuse + specular) * object_color;
	frag_color = vec4 (result, 1.0);
}
@end


@vs vs_grid
in vec3 position;

out mat4 frag_view;
out mat4 frag_projection;
out vec3 near_point;
out vec3 far_point;

uniform vs_grid_params
{
	mat4 view;
	mat4 projection;
	mat4 model;
	vec3 camera_position;
};

vec3 unproject_point (float x, float y, float z, mat4 view, mat4 projection)
{
	mat4 view_inverse = inverse (view);
	mat4 projection_inverse = inverse (projection);
	vec4 unprojected_point = view_inverse * projection_inverse * vec4 (x, y, z, 1.0);
	
	return unprojected_point.xyz / unprojected_point.w;
}

void main ()
{
	near_point = unproject_point (position.x, position.y, 0.0, view, projection).xyz;
	far_point = unproject_point (position.x, position.y, 1.0, view, projection).xyz;
	frag_view = view;
	frag_projection = projection;

	gl_Position = vec4 (position, 1.0);
}
@end


@fs fs_grid
in mat4 frag_view;
in mat4 frag_projection;
in vec3 near_point;
in vec3 far_point;

uniform fs_grid_params
{
	float grid_height;
};

out vec4 frag_color;

vec4 grid (vec3 frag_position_3d, float scale)
{
	// dont want the grid to be infinite?
	// 	uncomment this bit, set your boundaries to whatever you want
	//if (frag_position_3d.x > 10
	//	|| frag_position_3d.x < -10
	//	|| frag_position_3d.z > 10
	//	|| frag_position_3d.z < -10)
	//{
	//	return vec4 (0, 0, 0, 0);
	//}

	vec2 coord = frag_position_3d.xz * scale;
	vec2 derivative = fwidth (coord);
	vec2 grid = abs (fract (coord - 0.5) - 0.5) / derivative;
	float line = min (grid.x, grid.y);
	float minimum_z = min (derivative.y, 1);
	float minimum_x = min (derivative.x, 1);
	vec4 color = vec4 (0.2, 0.2, 0.2, 1.0 - min (line, 1.0));

	// z axis color
	//if (frag_position_3d.x > -0.1 * minimum_x
	//	&& frag_position_3d.x < 0.1 * minimum_x)
	//{
	//	color.z = 1.0;
	//}

	//// x axis color
	//if (frag_position_3d.z > -0.1 * minimum_z
	//	&& frag_position_3d.z < 0.1 * minimum_z)
	//{
	//	color.x = 1.0;
	//}
	
	return color;
}

float compute_depth (vec3 position)
{
	vec4 clip_space_position = frag_projection * frag_view * vec4 (position.xyz, 1.0);

	// the depth calculation in the original article is for vulkan
	// the depth calculation for opengl is:
	// 	(far - near) * 0.5 * ndc_depth + (far + near) * 0.5
	// 	far = 1.0  (opengl max depth)
	// 	near = 0.0  (opengl min depth)
	//		ndc_depth = clip_space_position.z / clip_space_position.w
	//	since our near and far are fixed, we can reduce the above formula to the following
	return 0.5 + 0.5 * (clip_space_position.z / clip_space_position.w);
	// this could also be (ndc_depth + 1.0) * 0.5
}

float compute_linear_depth (vec3 position)
{
	float near = 0.5;
	float far = 100;
	vec4 clip_space_position = frag_projection * frag_view * vec4 (position.xyz, 1.0);
	float clip_space_depth = (clip_space_position.z / clip_space_position.w) * 2.0 - 1.0;
	float linear_depth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near));

	return linear_depth / far;
}

void main ()
{
	float t = (grid_height - near_point.y) / (far_point.y - near_point.y);
	vec3 frag_position_3d = near_point + t * (far_point - near_point);

	gl_FragDepth = compute_depth (frag_position_3d);

	float linear_depth = compute_linear_depth (frag_position_3d);
	float fading = max (0, (0.5 - linear_depth));

	frag_color = (grid (frag_position_3d, 1) + grid (frag_position_3d, 1)) * float(t > 0);
	frag_color.a *= fading;
}
@end

@program cube vs_cube fs_cube
@program grid vs_grid fs_grid
