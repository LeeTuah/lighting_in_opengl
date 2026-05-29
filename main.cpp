# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include "include/camera.hpp"
# include "include/shaders.hpp"
# include "include/texture.hpp"

# include <string>

# define TOTAL_POINT_LIGHTS 4

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

bool first_mouse_input = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float delta_time = 0.0f;
float last_time = 0.0f;

float last_mouse_x = SCR_WIDTH / 2;
float last_mouse_y = SCR_HEIGHT / 2;

bool flashlight = false;

float last_flashlight_time = 0.0f;
float flashlight_cooldown = 0.35f;

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.process_keyboard(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.process_keyboard(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.process_keyboard(LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.process_keyboard(RIGHT, delta_time);

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

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting in OpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader lamp_shader("lamplight.vert", "lamplight.frag");
	lamp_shader.use();

	Shader main_shader("lightingtest.vert", "lightingtest.frag");
	main_shader.use();
	
	const unsigned int stride = 8;
	float rect_vertices[] = {
        // positions          // normals           // tex coord
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

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

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

	glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

	glm::vec3 point_light_positions[] = {
		glm::vec3( 0.7f, 0.2f, 2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3( 0.0f, 0.0f, -3.0f)
	};

	const float stretch[] = {1.0f, 1.0f, 1.0f}; // x, y, z
    float triangle_vertices[] = {
        // bottom face
        +stretch[0], 0, 0,
        -stretch[0], 0, 0,
        0, 0, stretch[2],

        // top face -xz
        -stretch[0], 0, 0,
        0, 0, stretch[2],
        0, stretch[1], 0,

        // top face -z
        +stretch[0], 0, 0,
        -stretch[0], 0, 0,
        0, stretch[1], 0,

        // top face xz
        +stretch[0], 0, 0,
        0, 0, stretch[2],
        0, stretch[1], 0
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

	glClearColor(0.2f, 0.2f, 0.4f, 1.0f);

	glm::vec3 object_color(0.1f, 0.3f, 0.6f);
	glm::vec3 lamp_color(1.0f, 1.0f, 1.0f);

	Texture2D container_tex("assets/container2.png");
	Texture2D container_spec_tex("assets/container2_specular.png");
	// Texture2D matrix_tex("assets/matrix.jpg");

	while (not glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		float current_time = glfwGetTime();
		delta_time = current_time - last_time;
		last_time = current_time;

		process_input(window);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view(1.0f);
		view = camera.get_view_matrix();

		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 model(1.0f);

		lamp_shader.use();
		lamp_shader.set_matrix4("view", view);
		lamp_shader.set_matrix4("projection", projection);

		lamp_shader.set_float3("lamp_color", lamp_color);

		glBindVertexArray(VAO);

		for (int i = 0; i < TOTAL_POINT_LIGHTS; i++){
			model = glm::mat4(1.0f);
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2f));

			lamp_shader.set_matrix4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		main_shader.use();
		main_shader.set_matrix4("view", view);
		main_shader.set_matrix4("projection", projection);

		main_shader.set_float3("viewer_pos", camera.position);
		
		glActiveTexture(GL_TEXTURE0);
		container_tex.bind();
		
		glActiveTexture(GL_TEXTURE1);
		container_spec_tex.bind();
		
		// glActiveTexture(GL_TEXTURE2);
		// matrix_tex.bind();
		
		main_shader.set_int("m1.diffuse", 0);
		main_shader.set_int("m1.specular", 1);
		// main_shader.set_int("m1.emission", 2);
		main_shader.set_float("m1.shininess", 32.0f);
		

		main_shader.set_float3("sun.direction", glm::vec3(-0.2f, -1.0f, -0.3f));

		main_shader.set_float3("sun.ambient", glm::vec3(0.2f));
		main_shader.set_float3("sun.diffuse", glm::vec3(0.6f));
		main_shader.set_float3("sun.specular", glm::vec3(1.0f));

		for (int i = 0; i < TOTAL_POINT_LIGHTS; i++) {
			std::string point_name = "point_lights[" + std::to_string(i) + "].";
			
			std::string position = point_name + "position";

			std::string constant = point_name + "constant";
			std::string linear = point_name + "linear";
			std::string quadratic = point_name + "quadratic";

			std::string ambient = point_name + "ambient";
			std::string diffuse = point_name + "diffuse";
			std::string specular = point_name + "specular";

			main_shader.set_float3(position.c_str(), point_light_positions[i]);

			main_shader.set_float(constant.c_str(), 1.0f);
			main_shader.set_float(linear.c_str(), 0.040f);
			main_shader.set_float(quadratic.c_str(), 0.0055f);

			main_shader.set_float3(ambient.c_str(), glm::vec3(0.2f));
			main_shader.set_float3(diffuse.c_str(), glm::vec3(0.6f));
			main_shader.set_float3(specular.c_str(), glm::vec3(1.0f));
		}

		main_shader.set_float3("flashlight.position", camera.position);
		main_shader.set_float3("flashlight.direction", camera.front);

		main_shader.set_float("flashlight.inner_cutoff", flashlight ? glm::cos(glm::radians(12.5f)) : glm::cos(glm::radians(0.0f)));
		main_shader.set_float("flashlight.outer_cutoff", flashlight ? glm::cos(glm::radians(17.5f)) : glm::cos(glm::radians(0.0f)));

		main_shader.set_float("flashlight.constant", 1.0f);
		main_shader.set_float("flashlight.linear", 0.040f);
		main_shader.set_float("flashlight.quadratic", 0.0055f);

		main_shader.set_float3("flashlight.ambient", glm::vec3(0.2f));
		main_shader.set_float3("flashlight.diffuse", glm::vec3(0.6f));
		main_shader.set_float3("flashlight.specular", glm::vec3(1.0f));

		for (int i = 0; i < 10; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);
			model = glm::rotate(model, glm::radians(15.0f * i * (float)sin(glfwGetTime())), glm::normalize(glm::vec3(1.0f)));
			main_shader.set_matrix4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	glfwTerminate();
	return 0;
}