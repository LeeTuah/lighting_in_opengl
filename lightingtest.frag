# version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
};

in vec3 normal;
in vec3 frag_pos;
in vec3 lamp_pos;

out vec4 FragColor;

uniform vec3 lamp_color;
uniform vec3 object_color;

uniform Material m1;
uniform Light lamp;

void main() {
	vec3 viewer_pos = vec3(0, 0, 0);
	
	vec3 ambient_light = lamp_color * lamp.ambient * m1.ambient;

	vec3 lamp_dirn = normalize(lamp_pos - frag_pos);
	float diff_dot_value = max(dot(normal, lamp_dirn), 0);
	vec3 diffuse_light = lamp_color * lamp.diffuse * (diff_dot_value * m1.diffuse);

	vec3 viewer_dirn = normalize(viewer_pos - frag_pos);
	vec3 reflected_dirn = reflect(-lamp_dirn, normal);
	float specular_dot_value = pow(max(dot(viewer_dirn, reflected_dirn), 0.0f), m1.shininess);
	vec3 specular_light = lamp_color * lamp.specular * (specular_dot_value * m1.specular);

	vec3 result = (ambient_light + diffuse_light + specular_light);
	FragColor = vec4(result, 1.0f);
}