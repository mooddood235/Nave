#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"

class Mesh{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Mesh();
	void Draw();
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	void GenerateMesh();
};

