#include "Texture.h"
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <iostream>

Texture::Texture() {
	this->id = 0;
	this->path = "default/texture";
}

Texture::Texture(unsigned int id, std::string path) {
	this->id = id;
	this->path = path;
}
Texture::Texture(std::string path, aiTextureType type) {

	this->path = path;
	glGenTextures(1, &id);

	int width, height, numChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);

	if (!data) {
		std::cout << "Texture at path <" << path << "> failed to load." << std::endl;
		stbi_image_free(data);
		exit(-1);
	}

	GLenum inFormat = GL_RED, outFormat = GL_RED;

	if (numChannels == 3) {
		if (type == aiTextureType_DIFFUSE) inFormat = GL_SRGB;
		else inFormat = GL_RGB;
		outFormat = GL_RGB;
	}
	else if (numChannels == 4) {
		if (type == aiTextureType_DIFFUSE) inFormat = GL_SRGB_ALPHA;
		else inFormat = GL_RGBA;
		outFormat = GL_RGBA;
	}
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, outFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float borderColor[] = { 0, 0, 0, 0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}