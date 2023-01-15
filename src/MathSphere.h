#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class MathSphere : public GameObject{
public:
	MathSphere();
	MathSphere(glm::vec3 position, float radius);
	float GetRadius();
private:
	float radius = 1.0f;
};

