# version 330 core

layout (location = 0) in vec3 vec_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 frag_pos;
out vec2 tex_coords;

void main() {
	// mat4 vm = view * model;

	mat3 normal_matrix = mat3(transpose(inverse(model)));
	normal = normalize(normal_matrix * a_normal);

	frag_pos = vec3(model * vec4(vec_pos, 1.0f));

	tex_coords = a_tex_coords;

	gl_Position = projection * view * model * vec4(vec_pos, 1.0f);
}