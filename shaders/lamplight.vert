# version 330 core

layout (location = 0) in vec3 vec_pos;
layout (location = 2) in vec2 a_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tex_coords;

void main() {
	tex_coords = a_tex_coords;
	gl_Position = projection * view * model * vec4(vec_pos, 1.0f);
}