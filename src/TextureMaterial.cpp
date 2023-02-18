#include "TextureMaterial.h"

TextureMaterial::TextureMaterial() {
	albedoTexture = Texture();
	roughnessTexture = Texture();
	metalnessTexture = Texture();
	emissionTexture = Texture();
	normalsTexture = Texture();
}

TextureMaterial::TextureMaterial(Texture diffuseTexture, Texture roughnessTexture, Texture metalnessTexture, Texture emissionTexture, Texture normalsTexture) {
	this->albedoTexture = diffuseTexture;
	this->roughnessTexture = roughnessTexture;
	this->metalnessTexture = metalnessTexture;
	this->emissionTexture = emissionTexture;
	this->normalsTexture = normalsTexture;
}