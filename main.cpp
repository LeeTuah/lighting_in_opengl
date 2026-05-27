# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include "include/camera.hpp"
# include "include/shaders.hpp"
# include "include/texture.hpp"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

bool first_mouse_input = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float delta_time = 0.0f;
float last_time = 0.0f;

float flashlight_cooldown = 1.0f;
float flashlight_start = 0.0f;

float last_mouse_x = SCR_WIDTH / 2;
float last_mouse_y = SCR_HEIGHT / 2;

bool flashlight = false;

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.process_keyboard(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.process_keyboard(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.process_keyboard(LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.process_keyboard(RIGHT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		float current_flash_time = glfwGetTime();
		if (current_flash_time - flashlight_start > flashlight_cooldown) {
			flashlight = not flashlight;
			flashlight_start = current_flash_time;
		}
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

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	glClearColor(0.2f, 0.2f, 0.4f, 1.0f);

	glm::vec3 object_color(0.1f, 0.3f, 0.6f);
	glm::vec3 lamp_color(1.0f, 1.0f, 1.0f);

	glm::vec3 lamp_pos(1.2f, 1.0f, 2.0f);
	glm::vec3 object_pos(0.0f, 0.0f, 6.7f);

	Texture2D container_tex("assets/container2.png");
	Texture2D container_spec_tex("assets/container2_specular.png");

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

		lamp_shader.use();
		lamp_shader.set_matrix4("view", view);
		lamp_shader.set_matrix4("projection", projection);

		lamp_shader.set_float3("lamp_color", lamp_color);

		glBindVertexArray(VAO);

		glm::mat4 model(1.0f);
		model = glm::translate(model, lamp_pos);
		model = glm::scale(model, glm::vec3(0.2f));

		lamp_shader.set_matrix4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (flashlight) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, camera.position);
			model = glm::scale(model, glm::vec3(0.001f));

			lamp_shader.set_matrix4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		main_shader.use();
		main_shader.set_matrix4("view", view);
		main_shader.set_matrix4("projection", projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, object_pos);
		main_shader.set_matrix4("model", model);

		main_shader.set_float3("lamp_posn", lamp_pos);
		main_shader.set_float3("lamp_color", lamp_color);
		main_shader.set_float3("object_color", object_color);
		
		glActiveTexture(GL_TEXTURE0);
		container_tex.bind();

		glActiveTexture(GL_TEXTURE1);
		container_spec_tex.bind();

		main_shader.set_int("m1.diffuse", 0);
		main_shader.set_int("m1.specular", 1);
		main_shader.set_float("m1.shininess", 32.0f);

		main_shader.set_float3("lamp.ambient", glm::vec3(0.2f));
		main_shader.set_float3("lamp.diffuse", glm::vec3(0.6f));
		main_shader.set_float3("lamp.specular", glm::vec3(1.0f));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glfwTerminate();
	return 0;
}