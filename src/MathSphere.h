#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Material.h"

class MathSphere : public GameObject{
public:
	MathSphere();
	MathSphere(glm::vec3 position, float radius, Material material);
	MathSphere(Material material);
	float GetRadius();

	Material material = Material();

private:
	float radius = 1.0f;
};

