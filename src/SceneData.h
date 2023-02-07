#pragma once
#include <vector>
#include "Vertex.h"

struct SceneData{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	SceneData(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
};

