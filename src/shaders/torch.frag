# version 330 core

in vec2 tex_coords;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
	vec4 tex = texture(tex, tex_coords);

	if (tex.a < 1.0) discard;

	FragColor = tex * vec4(0.9f, 0.9f, 0.9f, 1.0f);
}