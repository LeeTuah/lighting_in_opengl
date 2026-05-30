# ifndef TEXTURES_HPP_
# define TEXTURES_HPP_

# define STB_IMAGE_IMPLEMENTATION

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include "stb_image.h"

# include <iostream>
# include <vector>
# include <string>

class Texture2D{
public:
	unsigned int ID;

	Texture2D(const char* texture_path);
	void bind();
};

Texture2D::Texture2D(const char* texture_path) {
	int width, height, nr_channels;

	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(texture_path, &width, &height, &nr_channels, 0);
	if (data) {
		unsigned int format;
	
		if (nr_channels == 1) format = GL_RED;
		else if (nr_channels == 3) format = GL_RGB;
		else if (nr_channels == 4) format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else std::cout << "[!] Failed to load texture " << texture_path << std::endl;

	stbi_image_free(data);
}

void Texture2D::bind() {
	glBindTexture(GL_TEXTURE_2D, ID);
}

class Texture3D{};

# endif