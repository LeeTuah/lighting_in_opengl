# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include "include/camera.hpp"
# include "include/shaders.hpp"
# include "include/texture.hpp"

# include <string>
# include <algorithm>
# include <iterator>
# include <random>
# include <vector>

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

const float GROUND_LEVEL = -3.0f;

bool first_mouse_input = true;

Camera camera(glm::vec3(9.0f, GROUND_LEVEL + 2, 13.0f));

float delta_time = 0.0f;
float last_time = 0.0f;

float last_mouse_x = SCR_WIDTH / 2;
float last_mouse_y = SCR_HEIGHT / 2;

bool flashlight = false;

float last_flashlight_time = 0.0f;
float flashlight_cooldown = 0.35f;

glm::vec3 torch_color = glm::vec3(1.0f, 0.4f, 0.1f);
glm::vec3 torch_coords[] = {
	glm::vec3(9.0f, GROUND_LEVEL + 2, 7.0f),
	glm::vec3(9.0f, GROUND_LEVEL + 2, 7.0f),
	glm::vec3(9.0f, GROUND_LEVEL + 2, 7.0f),
	glm::vec3(9.0f, GROUND_LEVEL + 2, 7.0f),

	glm::vec3(3.0f, GROUND_LEVEL + 1, 3.0f),
	glm::vec3(17.0f, GROUND_LEVEL + 1, 3.0f),
	glm::vec3(3.0f, GROUND_LEVEL + 1, 17.0f),
	glm::vec3(17.0f, GROUND_LEVEL + 1, 17.0f),
	glm::vec3(13.0f, GROUND_LEVEL + 1, 9.0f),
	glm::vec3(6.0f, GROUND_LEVEL + 1, 14.0f),
	glm::vec3(9.0f, GROUND_LEVEL + 1, 2.0f)
};

struct WindowData {
	glm::vec3 position;
	float rotation;
};

int random_number(int begin, int end){
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<> distrib(begin, end);
    return distrib(generator);
}

void process_input(GLFWwindow* window) {
	bool print_camera_coords = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.process_keyboard(FORWARD, delta_time, print_camera_coords);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.process_keyboard(BACKWARD, delta_time, print_camera_coords);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.process_keyboard(LEFT, delta_time, print_camera_coords);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.process_keyboard(RIGHT, delta_time, print_camera_coords);

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		float current_time = glfwGetTime();
		if (current_time - last_flashlight_time <= flashlight_cooldown) return;

		flashlight = not flashlight;
		last_flashlight_time = current_time;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
	if (first_mouse_input) {
		last_mouse_x = x_pos;
		last_mouse_y = y_pos;
		first_mouse_input = false;
	}

	float x_offset = x_pos - last_mouse_x;
	float y_offset = last_mouse_y - y_pos;

	last_mouse_x = x_pos;
	last_mouse_y = y_pos;
	camera.process_mouse_movement(x_offset, y_offset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void send_model_matrix(
	Shader shader, glm::vec3 translate, glm::vec3 scale = glm::vec3(1.0f), 
	float rotation_angle = 0.0f, glm::vec3 rotation_axis = glm::vec3(0.0f)) {
	glm::mat4 model(1.0f);

	model = glm::translate(model, translate);
	if (scale != glm::vec3(1.0f))
		model = glm::scale(model, scale);
	if (rotation_angle != 0.0f)
		model = glm::rotate(model, glm::radians(rotation_angle), glm::normalize(rotation_axis));

	shader.set_matrix4("model", model);
}

void load_lights(Shader shader) {
	shader.set_float3("sun.direction", glm::vec3(-0.3f, -1.0f, -0.7f));

	shader.set_float3("sun.ambient", glm::vec3(0.05f, 0.05f, 0.15f)); 
	shader.set_float3("sun.diffuse", glm::vec3(0.1f, 0.15f, 0.3f));   
	shader.set_float3("sun.specular", glm::vec3(0.2f, 0.2f, 0.3f));

	shader.set_float3("sun.color", glm::vec3(1.0f));

	for (size_t i = 0; i < std::size(torch_coords); i++) {
		std::string torch_name = "point_lights[" + std::to_string(i) + "].";

		shader.set_float3((torch_name + "position").c_str(), torch_coords[i]);

		shader.set_float((torch_name + "constant").c_str(), 1.0f);
		shader.set_float((torch_name + "linear").c_str(), 0.35f);
		shader.set_float((torch_name + "quadratic").c_str(), 0.44f);

		shader.set_float3((torch_name + "ambient").c_str(), glm::vec3(0.1f));
		shader.set_float3((torch_name + "diffuse").c_str(), glm::vec3(0.9f));
		shader.set_float3((torch_name + "specular").c_str(), glm::vec3(1.0f));

		shader.set_float3((torch_name + "color").c_str(), torch_color);
	}

	shader.set_float3("flashlight.position", camera.position);
	shader.set_float3("flashlight.direction", camera.front);

	shader.set_float("flashlight.inner_cutoff", flashlight ? glm::cos(glm::radians(9.5f)) : glm::cos(glm::radians(0.0f)));
	shader.set_float("flashlight.outer_cutoff", flashlight ? glm::cos(glm::radians(11.5f)) : glm::cos(glm::radians(0.0f)));

	shader.set_float("flashlight.constant", 1.0f);
	shader.set_float("flashlight.linear", 0.022f);
	shader.set_float("flashlight.quadratic", 0.0019f);

	shader.set_float3("flashlight.ambient", glm::vec3(0.2f));
	shader.set_float3("flashlight.diffuse", glm::vec3(0.6f));
	shader.set_float3("flashlight.specular", glm::vec3(1.0f));

	shader.set_float3("flashlight.color", glm::vec3(1.0f));
}

int main() {
	camera.allow_flight = false;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "this is a window", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader lamp_shader("shaders/lamplight.vert", "shaders/lamplight.frag");
	lamp_shader.use();

	Shader flat_shader("shaders/flat.vert", "shaders/flat.frag");
	flat_shader.use();

	Shader tall_grass_shader("shaders/flat.vert", "shaders/tallgrass.frag");
	tall_grass_shader.use();

	Shader torch_shader("shaders/flat.vert", "shaders/torch.frag");
	torch_shader.use();

	Shader grass_block_shader("shaders/grass_blocks.vert", "shaders/lightingtest.frag");
	grass_block_shader.use();

	Shader main_shader("shaders/lightingtest.vert", "shaders/lightingtest.frag");
	main_shader.use();
	
	const unsigned int stride = 8;
	float rect_vertices[] = {
		// vertex coords      // normals           // tex coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f, 
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f, 
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f, 
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 

		
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	const unsigned int flat_stride = 5;
	float flat_vertices[] = {
		// vertex			// tex
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  
		0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 

		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.0f, 1.0f, 1.0f  
	};

	int FIELD_SIZE = 20;
	int index = 0;
	glm::vec3 grass_block_coordinates[FIELD_SIZE * FIELD_SIZE];

	for (int i = 0; i < FIELD_SIZE; i++) {
		for (int j = 0; j < FIELD_SIZE; j++) {
			grass_block_coordinates[index] = glm::vec3(float(i), GROUND_LEVEL, float(j));
			index += 1;
		}
	}

	const int TOWER_HEIGHT = 10;
	glm::vec3 tower_base_positions[] = {
		glm::vec3(6.0f, GROUND_LEVEL + 1, 6.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 1, 7.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 1, 8.0f),

		glm::vec3(7.0f, GROUND_LEVEL + 1, 9.0f),

		glm::vec3(8.0f, GROUND_LEVEL + 1, 10.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 1, 10.0f),
		glm::vec3(10.0f, GROUND_LEVEL + 1, 10.0f),

		glm::vec3(11.0f, GROUND_LEVEL + 1, 9.0f),

		glm::vec3(12.0f, GROUND_LEVEL + 1, 8.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 1, 7.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 1, 6.0f),

		glm::vec3(11.0f, GROUND_LEVEL + 1, 5.0f),

		glm::vec3(10.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(8.0f, GROUND_LEVEL + 1, 4.0f),

		glm::vec3(7.0f, GROUND_LEVEL + 1, 5.0f),
	};

	glm::vec3 tower_ignore_blocks[] = {
		glm::vec3(9.0f, GROUND_LEVEL + 1, 10.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 2, 10.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 2, 7.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 3, 7.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 2, 4.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 3, 4.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 2, 7.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 3, 7.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 10, 6.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 10, 7.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 10, 10.0f),
		glm::vec3(10.0f, GROUND_LEVEL + 10, 10.0f),
		glm::vec3(11.0f, GROUND_LEVEL + 10, 9.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 10, 8.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 10, 7.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 9, 6.0f),
		glm::vec3(10.0f, GROUND_LEVEL + 9, 10.0f),
		glm::vec3(11.0f, GROUND_LEVEL + 9, 9.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 9, 8.0f),
		glm::vec3(11.0f, GROUND_LEVEL + 8, 9.0f)
	};

	const int TREE_HEIGHT = 2;
	glm::vec3 birch_tree_coords[] = {
		glm::vec3(18.0f, GROUND_LEVEL + 1, 10.0f),
		glm::vec3(14.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(2.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(3.0f, GROUND_LEVEL + 1, 13.0f),
		glm::vec3(11.0f, GROUND_LEVEL + 1, 13.0f),
	};

	glm::vec3 tall_grass_coords[] = {
		glm::vec3(4.0f, GROUND_LEVEL + 1, 10.0f),
		glm::vec3(1.0f, GROUND_LEVEL + 1, 3.0f),
		glm::vec3(2.0f, GROUND_LEVEL + 1, 7.0f),
		glm::vec3(4.0f, GROUND_LEVEL + 1, 2.0f),
		glm::vec3(5.0f, GROUND_LEVEL + 1, 9.0f),
		glm::vec3(1.0f, GROUND_LEVEL + 1, 15.0f),
		glm::vec3(3.0f, GROUND_LEVEL + 1, 17.0f),
		glm::vec3(5.0f, GROUND_LEVEL + 1, 12.0f),
		glm::vec3(7.0f, GROUND_LEVEL + 1, 16.0f),
		glm::vec3(9.0f, GROUND_LEVEL + 1, 13.0f),
		glm::vec3(10.0f, GROUND_LEVEL + 1, 18.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 1, 16.0f),
		glm::vec3(14.0f, GROUND_LEVEL + 1, 19.0f),
		glm::vec3(14.0f, GROUND_LEVEL + 1, 3.0f),
		glm::vec3(16.0f, GROUND_LEVEL + 1, 6.0f),
		glm::vec3(18.0f, GROUND_LEVEL + 1, 1.0f),
		glm::vec3(19.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(15.0f, GROUND_LEVEL + 1, 15.0f),
		glm::vec3(18.0f, GROUND_LEVEL + 1, 8.0f),
		glm::vec3(19.0f, GROUND_LEVEL + 1, 12.0f),
		glm::vec3(17.0f, GROUND_LEVEL + 1, 18.0f)
	};

	glm::vec3 short_grass_coords[] = {
		glm::vec3(1.0f, GROUND_LEVEL + 1, 1.0f),
		glm::vec3(2.0f, GROUND_LEVEL + 1, 2.0f),
		glm::vec3(2.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(3.0f, GROUND_LEVEL + 1, 1.0f),
		glm::vec3(4.0f, GROUND_LEVEL + 1, 3.0f),
		glm::vec3(5.0f, GROUND_LEVEL + 1, 5.0f),
		glm::vec3(1.0f, GROUND_LEVEL + 1, 6.0f),
		glm::vec3(3.0f, GROUND_LEVEL + 1, 7.0f),
		glm::vec3(1.0f, GROUND_LEVEL + 1, 12.0f),
		glm::vec3(3.0f, GROUND_LEVEL + 1, 14.0f),
		glm::vec3(2.0f, GROUND_LEVEL + 1, 15.0f),
		glm::vec3(4.0f, GROUND_LEVEL + 1, 13.0f),
		glm::vec3(4.0f, GROUND_LEVEL + 1, 18.0f),
		glm::vec3(1.0f, GROUND_LEVEL + 1, 19.0f),
		glm::vec3(5.0f, GROUND_LEVEL + 1, 17.0f),
		glm::vec3(6.0f, GROUND_LEVEL + 1, 15.0f),
		glm::vec3(7.0f, GROUND_LEVEL + 1, 13.0f),
		glm::vec3(8.0f, GROUND_LEVEL + 1, 18.0f),
		glm::vec3(10.0f, GROUND_LEVEL + 1, 15.0f),
		glm::vec3(11.0f, GROUND_LEVEL + 1, 18.0f),
		glm::vec3(12.0f, GROUND_LEVEL + 1, 14.0f),
		glm::vec3(13.0f, GROUND_LEVEL + 1, 17.0f),
		glm::vec3(14.0f, GROUND_LEVEL + 1, 2.0f),
		glm::vec3(15.0f, GROUND_LEVEL + 1, 5.0f),
		glm::vec3(16.0f, GROUND_LEVEL + 1, 1.0f),
		glm::vec3(17.0f, GROUND_LEVEL + 1, 4.0f),
		glm::vec3(18.0f, GROUND_LEVEL + 1, 2.0f),
		glm::vec3(19.0f, GROUND_LEVEL + 1, 6.0f),
		glm::vec3(15.0f, GROUND_LEVEL + 1, 10.0f),
		glm::vec3(16.0f, GROUND_LEVEL + 1, 12.0f),
		glm::vec3(17.0f, GROUND_LEVEL + 1, 9.0f),
		glm::vec3(18.0f, GROUND_LEVEL + 1, 11.0f),
		glm::vec3(17.0f, GROUND_LEVEL + 1, 15.0f),
		glm::vec3(19.0f, GROUND_LEVEL + 1, 14.0f),
		glm::vec3(18.0f, GROUND_LEVEL + 1, 18.0f),
		glm::vec3(15.0f, GROUND_LEVEL + 1, 19.0f)
	};

	std::vector<WindowData> window_locations;
	window_locations.push_back({glm::vec3(6.0f, GROUND_LEVEL + 2, 7.0f), 90.0f});
	window_locations.push_back({glm::vec3(6.0f, GROUND_LEVEL + 3, 7.0f), 90.0f});
	window_locations.push_back({glm::vec3(9.0f, GROUND_LEVEL + 2, 4.0f), 0.0f});
	window_locations.push_back({glm::vec3(9.0f, GROUND_LEVEL + 3, 4.0f), 0.0f});
	window_locations.push_back({glm::vec3(12.0f, GROUND_LEVEL + 2, 7.0f), 90.0f});
	window_locations.push_back({glm::vec3(12.0f, GROUND_LEVEL + 3, 7.0f), 90.0f});

	int start_height = 1;
	int end_height = 3;
	int birch_heights[] = {
		random_number(start_height, end_height),
		random_number(start_height, end_height),
		random_number(start_height, end_height),
		random_number(start_height, end_height),
		random_number(start_height, end_height)
	};

	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	unsigned int grass_block_VAO, grass_block_VBO, grass_block_instance_VBO;

	glGenVertexArrays(1, &grass_block_VAO);
	glBindVertexArray(grass_block_VAO);

	glGenBuffers(1, &grass_block_VBO);
	glGenBuffers(1, &grass_block_instance_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, grass_block_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, grass_block_instance_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass_block_coordinates), &grass_block_coordinates[0], GL_STATIC_DRAW);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);

	unsigned int flatVAO, flatVBO;

	glGenVertexArrays(1, &flatVAO);
	glBindVertexArray(flatVAO);

	glGenBuffers(1, &flatVBO);
	glBindBuffer(GL_ARRAY_BUFFER, flatVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flat_vertices), flat_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, flat_stride * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, flat_stride * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glClearColor(0.05f, 0.08f, 0.14f, 1.0f);

	glm::vec3 object_color(0.1f, 0.3f, 0.6f);
	glm::vec3 lamp_color(1.0f, 1.0f, 1.0f);

	Texture2D cobblestone_diffuse("assets/cobblestone_diffuse.png");
	Texture2D cobblestone_specular("assets/cobblestone_specular.png");

	Texture2D grass_diffuse("assets/grass_diffuse.jpg");
	Texture2D grass_specular("assets/grass_specular.png");

	Texture2D sea_lantern_diffuse("assets/sea_lantern_diffuse.png");
	
	Texture2D birch_log("assets/birch_log.jpg");

	Texture2D moon_tex("assets/moon.png");
	Texture2D black("assets/black.png");

	Texture2D window_texture("assets/yellow_stained_glass.png");

	Texture2D short_grass("assets/short_grass.png");
	Texture2D tall_grass("assets/tall_grass.png");

	Texture2D redstone_torch("assets/redstone_torch.png");

	Texture2D gold_block("assets/gold_block.png");

	while (not glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		float current_time = glfwGetTime();
		delta_time = current_time - last_time;
		last_time = current_time;

		process_input(window);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::mat4 view(1.0f);
		view = camera.get_view_matrix();

		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 model(1.0f);

		flat_shader.use();
		flat_shader.set_matrix4("view", view);
		flat_shader.set_matrix4("projection", projection);

		glBindVertexArray(flatVAO);

		glActiveTexture(GL_TEXTURE0);
		moon_tex.bind();

		flat_shader.set_int("tex", 0);

		send_model_matrix(flat_shader, glm::vec3(9.0f, GROUND_LEVEL + 40, -51.0f), glm::vec3(15.0f), 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		grass_block_shader.use();
		glBindVertexArray(grass_block_VAO);

		grass_block_shader.set_matrix4("view", view);
		grass_block_shader.set_matrix4("projection", projection);

		grass_block_shader.set_int("allocated_point_lights", (int)std::size(torch_coords));
		grass_block_shader.set_float3("viewer_pos", camera.position);

		grass_block_shader.set_int("m1.diffuse", 0);
		grass_block_shader.set_int("m1.specular", 1);
		grass_block_shader.set_int("m1.overlay", 2);
		grass_block_shader.set_float("m1.shininess", 32.0f);

		load_lights(grass_block_shader);

		glActiveTexture(GL_TEXTURE0);
		grass_diffuse.bind();
		
		glActiveTexture(GL_TEXTURE1);
		grass_specular.bind();

		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, FIELD_SIZE * FIELD_SIZE);

		main_shader.use();

		main_shader.set_matrix4("view", view);
		main_shader.set_matrix4("projection", projection);

		main_shader.set_int("allocated_point_lights", (int)std::size(torch_coords));

		main_shader.set_float3("viewer_pos", camera.position);
		
		main_shader.set_int("m1.diffuse", 0);
		main_shader.set_int("m1.specular", 1);
		main_shader.set_int("m1.overlay", 2);
		main_shader.set_float("m1.shininess", 32.0f);
		
		load_lights(main_shader);

		glActiveTexture(GL_TEXTURE0);
		cobblestone_diffuse.bind();

		glActiveTexture(GL_TEXTURE1);
		cobblestone_specular.bind();

		for (int j = 0; j < TOWER_HEIGHT; j++) {
			for (size_t i = 0; i < std::size(tower_base_positions); i++) {
				glm::vec3 posn = tower_base_positions[i] + glm::vec3(0.0f, (float)j, 0.0f);

				if (std::find(std::begin(tower_ignore_blocks), std::end(tower_ignore_blocks), posn) != std::end(tower_ignore_blocks))
					continue;

				send_model_matrix(main_shader, posn);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		glActiveTexture(GL_TEXTURE0);
		gold_block.bind();

		send_model_matrix(main_shader, glm::vec3(9.0f, GROUND_LEVEL + 1, 7.0f));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glActiveTexture(GL_TEXTURE0);
		birch_log.bind();

		glActiveTexture(GL_TEXTURE1);
		black.bind();

		for (size_t i = 0; i < std::size(birch_tree_coords); i++) {
			for (int j = 0; j < TREE_HEIGHT + birch_heights[i]; j++) {
				send_model_matrix(main_shader, birch_tree_coords[i] + glm::vec3(0.0f, (float)j, 0.0f));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		send_model_matrix(main_shader, glm::vec3(10.0f, GROUND_LEVEL + 3, 13.0f));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		send_model_matrix(main_shader, glm::vec3(12.0f, GROUND_LEVEL + 3, 13.0f));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		flat_shader.use();
		flat_shader.set_int("tex", 0);

		glBindVertexArray(flatVAO);

		glActiveTexture(GL_TEXTURE0);
		window_texture.bind();

		std::sort(window_locations.begin(), window_locations.end(), [](WindowData a, WindowData b) {
			return glm::length(camera.position - a.position) > glm::length(camera.position - b.position);
		});

		for (auto window : window_locations) {
			send_model_matrix(flat_shader, window.position, glm::vec3(1.0f), window.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		tall_grass_shader.use();
		tall_grass_shader.set_matrix4("projection", projection);
		tall_grass_shader.set_matrix4("view", view);

		glActiveTexture(GL_TEXTURE0);
		tall_grass.bind();

		for (size_t i = 0; i < std::size(tall_grass_coords); i++) {
			send_model_matrix(tall_grass_shader, tall_grass_coords[i] + glm::vec3(0.0f, 0.3f, 0.5f), glm::vec3(1.0f, 2.0f, 1.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			send_model_matrix(tall_grass_shader, tall_grass_coords[i] + glm::vec3(0.0f, 0.3f, 0.5f), glm::vec3(1.0f, 2.0f, 1.0f), -45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glActiveTexture(GL_TEXTURE0);
		short_grass.bind();

		for (size_t i = 0; i < std::size(short_grass_coords); i++) {
			send_model_matrix(tall_grass_shader, short_grass_coords[i] - glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(1.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			send_model_matrix(tall_grass_shader, short_grass_coords[i] - glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(1.0f), -45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		torch_shader.use();
		torch_shader.set_matrix4("projection", projection);
		torch_shader.set_matrix4("view", view);

		torch_shader.set_int("tex", 0);

		glActiveTexture(GL_TEXTURE0);
		redstone_torch.bind();

		for (size_t i = 0; i < std::size(torch_coords); i++) {
			send_model_matrix(torch_shader, torch_coords[i] - glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(0.4, 0.5f, 0.4f));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			send_model_matrix(torch_shader, torch_coords[i] - glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(0.4, 0.5f, 0.4f), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	
	glfwTerminate();
	return 0;
}