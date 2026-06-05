# version 330 core

in vec2 tex_coords;

out vec4 FragColor;

uniform sampler2D text;
uniform vec3 text_color;

void main() {
	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(text, tex_coords).r);
	FragColor = vec4(text_color, 1.0f) * sampled;
}