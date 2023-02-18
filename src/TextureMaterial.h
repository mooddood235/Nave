#pragma once
#include "Texture.h"

struct TextureMaterial{
	Texture diffuseTexture;

	TextureMaterial();
	TextureMaterial(Texture diffuseTexture);
};

