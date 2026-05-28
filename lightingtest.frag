# version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	// sampler2D emission; // for exercise
	
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position; 
	// vec3 direction; // for global light (dirn of light)
	vec3 direction; // for spotlight (dirn of spotlight)
	float inner_cutoff;
	float outer_cutoff;

	// for attenuation light caster
	float constant;
	float linear;
	float quadratic;
};

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 FragColor;

uniform vec3 lamp_color;
uniform vec3 object_color;
uniform vec3 viewer_pos;

uniform Material m1;
uniform Light lamp;

void main() {
	// vec3 norm = normalize(normal);
	vec3 lamp_dirn = normalize(lamp.position - frag_pos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	// for attenuation light
	float dist = length(lamp.position - frag_pos);
	float attenuation = 1.0 / (lamp.constant + (lamp.linear * dist) + (lamp.quadratic * dist * dist));

	// for spotlight
	float theta = dot(normalize(-lamp.direction), lamp_dirn);
	float epsilon = lamp.inner_cutoff - lamp.outer_cutoff;
	float intensity = clamp((theta - lamp.outer_cutoff) / epsilon, 0.0f, 1.0f);
	
	vec3 ambient_light = lamp.ambient * texture(m1.diffuse, tex_coords).rgb * attenuation;

	// vec3 lamp_dirn = normalize(-lamp.direction);	// for global light
	float diff_dot_value = max(dot(normal, lamp_dirn), 0);
	vec3 diffuse_light = lamp.diffuse * diff_dot_value * texture(m1.diffuse, tex_coords).rgb * attenuation * intensity;


	vec3 viewer_dirn = normalize(viewer_pos - frag_pos);
	vec3 reflected_dirn = reflect(-lamp_dirn, normal);
	float specular_dot_value = pow(max(dot(viewer_dirn, reflected_dirn), 0.0f), m1.shininess);
	vec3 specular_light = lamp.specular * specular_dot_value * texture(m1.specular, tex_coords).rgb * attenuation * intensity;

	// vec3 emission_light = texture(m1.emission, tex_coords).rgb;

	result = ambient_light + diffuse_light + specular_light /* + emission_light */;
	FragColor = vec4(result, 1.0f);
}