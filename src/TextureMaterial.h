#pragma once
#include "Texture.h"

struct TextureMaterial{
	Texture albedoTexture;
	Texture roughnessTexture;
	Texture metalnessTexture;
	Texture emissionTexture;
	Texture normalsTexture;

	TextureMaterial();
	TextureMaterial(Texture diffuseTexture, Texture roughnessTexture, Texture metalnessTexture, Texture emissionTexture, Texture normalsTexture);
};

