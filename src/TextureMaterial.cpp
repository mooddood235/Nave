#include "TextureMaterial.h"

TextureMaterial::TextureMaterial() {
	diffuseTexture = Texture();
}

TextureMaterial::TextureMaterial(Texture diffuseTexture) {
	this->diffuseTexture = diffuseTexture;
}