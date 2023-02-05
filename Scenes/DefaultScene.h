#pragma once
#include <glm/glm.hpp>
#include "../src/Scene.h"
#include "../src/MathSphere.h"
#include "../src/Material.h"
#include "../src/Camera.h"
#include "../src/EnvironmentMap.h"
#include "../src/ShaderProgram.h"

class DefaultScene : public Scene{
public:
    void SetMathSpheres(ShaderProgram rayTraceShader);
private:
    MathSphere mathSpheres[4] = {
        MathSphere(Material(glm::vec3(1.0f, 0.0, 0.0f), 0.0f, 0.0f)),
        MathSphere(glm::vec3(-2.0f, 0.0, 0.0f), 1.0f, Material(glm::vec3(1.0f), 0.0f, 1.0f)),
        MathSphere(glm::vec3(-1.0f, 0.0f, -2.0f), 1.0f, Material(glm::vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.0f)),
        MathSphere(glm::vec3(0.0f, -51.0f, 0.0f), 50.0f, Material(glm::vec3(1.0), 1.0f, 0.0f))
    };
};

