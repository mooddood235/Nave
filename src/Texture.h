#pragma once
#include <string>
#include <assimp/scene.h>

struct Texture{
	unsigned int id;
	std::string path;

	Texture();
	Texture(std::string path, aiTextureType type);
	Texture(unsigned int id, std::string path);
};

