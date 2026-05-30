# ifndef CAMERA_HPP_
# define CAMERA_HPP_

# include <glad/glad.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>

# include <iostream>

enum camera_moves {
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT
};

const float YAW = -89.0f;
const float PITCH = 0.0f;
const float SPEED = 2.67f;
const float SENSITIVITY = 0.041f;
const float ZOOM = 55.0f;

class Camera {
public:
	glm::vec3 position;

	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	glm::vec3 world_up;

	float yaw;
	float pitch;

	float movement_speed;
	float sensitivity;
	float zoom;

	bool allow_flight;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 get_view_matrix();

	void process_keyboard(camera_moves direction, float delta_time, bool print_camera_coords = false);
	void process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch = true);

	void print_camera_coordinates();

private:
	void update_camera_vectors();
};

Camera::Camera(glm::vec3 position, glm::vec3 world_up) {
	this->position = position;
	this->world_up = world_up;

	yaw = YAW;
	pitch = PITCH;
	movement_speed = SPEED;
	sensitivity = SENSITIVITY;
	zoom = ZOOM;
	allow_flight = false;

	this->update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::process_keyboard(camera_moves direction, float delta_time, bool print_camera_coords) {
	float velocity = movement_speed * delta_time;
	glm::vec3 old_position = position;

	if (direction == FORWARD) position += front * velocity;
	else if (direction == BACKWARD) position -= front * velocity;
	else if (direction == RIGHT) position += right * velocity;
	else if (direction == LEFT) position -= right * velocity;

	if (not allow_flight) position.y = old_position.y;
	if (print_camera_coords) print_camera_coordinates();
}

void Camera::process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch) {
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	yaw += x_offset;
	pitch += y_offset;

	if (constrain_pitch) {
		if (pitch > 89.0f) pitch = 89.0f;
		else if (pitch < -89.0f) pitch = -89.0f;
	}

	update_camera_vectors();
}

void Camera::print_camera_coordinates() {
	std::cout << "Camera: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void Camera::update_camera_vectors() {
	glm::vec3 _front;
	_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	_front.y = sin(glm::radians(pitch));
	_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(_front);

	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}

# endif