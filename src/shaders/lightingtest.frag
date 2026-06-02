# version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	
	float shininess;
};

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

struct Spotlight {
	vec3 position;
	
	vec3 direction;
	float inner_cutoff;
	float outer_cutoff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 FragColor;

uniform vec3 viewer_pos;

uniform Material m1;

vec3 calc_directional_light(DirectionalLight light, Material material, vec2 tex_coord, vec3 normal, vec3 viewer_dirn) {
	vec3 light_direction = normalize(-light.direction);

	vec3 ambient_light = light.color * light.ambient * texture(material.diffuse, tex_coord).rgb;

	float diffuse_dot_value = max(dot(light_direction, normal), 0.0f);
	vec3 diffuse_light = light.color * light.diffuse * diffuse_dot_value * texture(material.diffuse, tex_coord).rgb;

	vec3 reflected_dirn = reflect(-light_direction, normal);
	float specular_dot_value = pow(max(dot(reflected_dirn, viewer_dirn), 0.0f), material.shininess);
	vec3 specular_light = light.specular * specular_dot_value * texture(material.specular, tex_coord).rgb;

	return (ambient_light + diffuse_light + specular_light);
}

vec3 calc_point_light(PointLight light, Material material, vec2 tex_coord, vec3 normal, vec3 fragment_pos, vec3 viewer_dirn) {
	vec3 light_direction = normalize(light.position - fragment_pos);

	float dist = length(light.position - fragment_pos);
	float attenuation = 1.0 / (light.constant + (light.linear * dist) + (light.quadratic * dist * dist));

	vec3 ambient_light = light.color * light.ambient * texture(material.diffuse, tex_coord).rgb * attenuation;

	float diffuse_dot_value = max(dot(light_direction, normal), 0.0f);
	vec3 diffuse_light = light.color * light.diffuse * diffuse_dot_value * texture(material.diffuse, tex_coord).rgb * attenuation;

	vec3 reflected_dirn = reflect(-light_direction, normal);
	float specular_dot_value = pow(max(dot(reflected_dirn, viewer_dirn), 0.0f), material.shininess);
	vec3 specular_light = light.specular * specular_dot_value * texture(material.specular, tex_coord).rgb * attenuation;

	return (ambient_light + diffuse_light + specular_light);
}

vec3 calc_spotlight(Spotlight light, Material material, vec2 tex_coord, vec3 normal, vec3 fragment_pos, vec3 viewer_dirn) {
	vec3 light_direction = normalize(-light.direction);
	vec3 light_dir_from_fragment = normalize(light.position - fragment_pos);

	float theta = dot(light_direction, light_dir_from_fragment);
	if (theta < light.outer_cutoff) return vec3(0.0f);

	float dist = length(light.position - fragment_pos);
	float attenuation = 1.0 / (light.constant + (dist * light.linear) + (dist * dist * light.quadratic));

	float epsilon = light.inner_cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);

	vec3 ambient_light = light.color * light.ambient * texture(material.diffuse, tex_coord).rgb * attenuation * intensity;

	float diffuse_dot_value = max(dot(light_dir_from_fragment, normal), 0.0f);
	vec3 diffuse_light = light.color * light.diffuse * diffuse_dot_value * texture(material.diffuse, tex_coord).rgb * attenuation * intensity;

	vec3 reflected_dirn = reflect(-light_dir_from_fragment, normal);
	float specular_dot_value = pow(max(dot(reflected_dirn, viewer_dirn), 0.0f), material.shininess);
	vec3 specular_light = light.specular * specular_dot_value * texture(material.specular, tex_coord).rgb * attenuation * intensity;

	return (ambient_light + diffuse_light + specular_light);
}

uniform DirectionalLight sun;
uniform PointLight lantern;
uniform Spotlight flashlight;

void main() {
	vec3 norm = normalize(normal);
	vec3 viewer_dirn = normalize(viewer_pos - frag_pos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	result += calc_directional_light(sun, m1, tex_coords, norm, viewer_dirn);
	result += calc_point_light(lantern, m1, tex_coords, norm, frag_pos, viewer_dirn);
	result += calc_spotlight(flashlight, m1, tex_coords, norm, frag_pos, viewer_dirn);
	
	FragColor = vec4(result, 1.0f);
}