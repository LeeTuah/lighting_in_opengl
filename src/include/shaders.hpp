# ifndef SHADER_HPP_
# define SHADER_HPP_

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

void print(const char* message, std::string end = "\n", bool flush = true) {
	std::cout << message << end;

	if (flush) std::cout << std::flush;
}

class Shader {
public:
	unsigned int ID;

	Shader(const char* vertex_shader_path, const char* fragment_shader_path);

	void use();

	void set_bool(const char* name, bool value);
	void set_int(const char* name, int value);
	void set_float(const char* name, float value);

	void set_float3(const char* name, float x, float y, float z);
	void set_float3(const char* name, glm::vec3 value);

	void set_float4(const char* name, float x, float y, float z, float w);
	void set_float4(const char* name, glm::vec4 value);

	void set_matrix4(const char* name, glm::mat4 value);
};

Shader::Shader(const char* vertex_shader_path, const char* fragment_shader_path) {
	std::string vertex_shader_str, fragment_shader_str;
	std::ifstream v_file, f_file;

	try {
		std::stringstream v_stream, f_stream;

		v_file.open(vertex_shader_path);
		f_file.open(fragment_shader_path);

		v_stream << v_file.rdbuf();
		f_stream << f_file.rdbuf();

		v_file.close();
		f_file.close();

		vertex_shader_str = v_stream.str();
		fragment_shader_str = f_stream.str();
	} catch (std::ifstream::failure &e) {
		print("[!] Could not read the shader files!");
	}

	const char* vertex_code = vertex_shader_str.c_str();
	const char* fragment_code = fragment_shader_str.c_str();

	unsigned int vertex_shader, fragment_shader;
	int success;
	char info_log[512];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_code, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (not success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		print("[!] Vertex shader failed to compile!");
		std::cout << info_log << std::endl;
		std::cout << vertex_shader_path << std::endl;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_code, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (not success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		print("[!] Fragment shader failed to compile!");
		std::cout << info_log << std::endl;
		std::cout << fragment_shader_path << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex_shader);
	glAttachShader(ID, fragment_shader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (not success) {
		glGetProgramInfoLog(ID, 512, NULL, info_log);
		print("[!] Shader program failed to link!");
		std::cout << info_log << std::endl;
		std::cout << vertex_shader_path << " and " << fragment_shader_path << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::set_bool(const char *name, bool value) {
	int location = glGetUniformLocation(ID, name);
	glUniform1i(location, value);
}

void Shader::set_int(const char* name, int value) {
	int location = glGetUniformLocation(ID, name);
	glUniform1i(location, value);
}

void Shader::set_float(const char* name, float value) {
	int location = glGetUniformLocation(ID, name);
	glUniform1f(location, value);
}

void Shader::set_float3(const char* name, float x, float y, float z) {
	int location = glGetUniformLocation(ID, name);
	glUniform3f(location, x, y, z);
}

void Shader::set_float3(const char* name, glm::vec3 value) {
	int location = glGetUniformLocation(ID, name);
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::set_float4(const char* name, float x, float y, float z, float w) {
	int location = glGetUniformLocation(ID, name);
	glUniform4f(location, x, y, z, w);
}

void Shader::set_float4(const char* name, glm::vec4 value) {
	int location = glGetUniformLocation(ID, name);
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::set_matrix4(const char* name, glm::mat4 value) {
	int location = glGetUniformLocation(ID, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

# endif