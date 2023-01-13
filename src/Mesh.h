#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "GameObject.h"
#include "Vertex.h"

class Mesh : public GameObject{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	void Draw();
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	void GenerateMesh();
};

