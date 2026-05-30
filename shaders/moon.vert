# version 330 core

layout (location = 0) in vec2 vec_pos;
layout (location = 1) in vec2 a_tex_coords;

out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	tex_coords = a_tex_coords;

	gl_Position = projection * view * model * vec4(vec_pos, 1.0f, 1.0f);
}