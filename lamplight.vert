# version 330 core

layout (location = 0) in vec3 vec_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(vec_pos, 1.0f);
}