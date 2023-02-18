#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "GameObject.h"
#include "Vertex.h"
#include "TextureMaterial.h"

class Mesh : public GameObject{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, TextureMaterial textureMaterial);
	void Draw();
	std::vector<Vertex> GetVertices();
	std::vector<unsigned int> GetIndices();
	TextureMaterial GetTextureMaterial();

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	TextureMaterial textureMaterial;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	void GenerateMesh();
};

