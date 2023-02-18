#include "Vertex.h"

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec3 tangent, glm::vec3 biTangent, glm::vec2 uv) {
	this->pos = pos;
	this->normal = normal;
	this->tangent = tangent;
	this->biTangent = biTangent;
	this->uv = uv;
}