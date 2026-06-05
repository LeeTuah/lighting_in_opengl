# version 330 core

layout (location = 0) in vec4 vertex;

out vec2 tex_coords;

uniform mat4 projection;

void main() {
	gl_Position = projection * vec4(vertex.x, vertex.y, 0.0f, 1.0f);
	tex_coords = vec2(vertex.z, vertex.w);
}