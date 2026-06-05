# ifndef TEXTURES_HPP_
# define TEXTURES_HPP_

# define STB_IMAGE_IMPLEMENTATION

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include "stb_image.h"

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include <ft2build.h>
# include FT_FREETYPE_H

# include <iostream>
# include <vector>
# include <string>
# include <map>

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

struct Character{
	unsigned int texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class character_class{
public:
	std::map<char, Character> characters;

	character_class(const char* font_path, int font_height = 48);
	void render_text(
		Shader s, std::string text, unsigned int VAO, unsigned int VBO, 
		float x, float y, float scale, glm::vec3 color, glm::mat4 projection
	);
};

character_class::character_class(const char* font_path, int font_height) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)){
		std::cout << "[!] Error: Could not initialize freetype!" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, font_path, 0, &face)){
		std::cout << "[!] Error: Could not load font!" << std::endl;
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, font_height);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
			std::cout << "[!] Error: Could not load a glyph!" << std::endl;
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), face->glyph->advance.x
		};
		characters.insert({c, character});
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void character_class::render_text(
	Shader shader, std::string text, unsigned int VAO, unsigned int VBO, 
	float x, float y, float scale, glm::vec3 color, glm::mat4 projection
) {
	shader.use();
	shader.set_matrix4("projection", projection);
	shader.set_float3("text_color", color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	for (auto ch : text) {
		Character _char = characters[ch];

		float x_pos = x + _char.bearing.x * scale;
		float y_pos = y - (_char.size.y - _char.bearing.y) * scale;

		float w = _char.size.x * scale;
		float h = _char.size.y * scale;

		float vertices[6][4] = {
			{x_pos    , y_pos + h, 0.0f, 0.0f},
			{x_pos    , y_pos    , 0.0f, 1.0f},
			{x_pos + w, y_pos    , 1.0f, 1.0f},
			{x_pos    , y_pos + h, 0.0f, 0.0f},
			{x_pos + w, y_pos    , 1.0f, 1.0f},
			{x_pos + w, y_pos + h, 1.0f, 0.0f}
		};

		glBindTexture(GL_TEXTURE_2D, _char.texture_id);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (_char.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

class Texture3D{};

# endif