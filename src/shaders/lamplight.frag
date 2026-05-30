# version 330 core

out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D lamp_texture;
uniform vec3 lamp_color;

void main() {
	vec3 result = lamp_color * texture(lamp_texture, tex_coords).rgb;
	FragColor = vec4(result, 1.0f);
}