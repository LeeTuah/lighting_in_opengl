# version 330 core

in vec2 tex_coords;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
	vec4 tex_color = texture(tex, tex_coords);

	if (tex_color.a < 0.1) discard;

	FragColor = tex_color;
}