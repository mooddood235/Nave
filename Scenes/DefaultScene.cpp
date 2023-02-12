#include "DefaultScene.h"
#include "../src/SceneData.h"

DefaultScene::DefaultScene() {
    mathSpheres = {
        MathSphere(Material(glm::vec3(1.0f, 0.0, 0.0f), 0.0f, 0.0f)),
        MathSphere(glm::vec3(-2.0f, 0.0, 0.0f), 1.0f, Material(glm::vec3(1.0f), 0.0f, 1.0f)),
        MathSphere(glm::vec3(-1.0f, 0.0f, -2.0f), 1.0f, Material(glm::vec3(0.0f, 0.0f, 1.0f), 0.05f, 0.0f)),
        MathSphere(glm::vec3(0.0f, -51.0f, 0.0f), 50.0f, Material(glm::vec3(1.0), 1.0f, 0.0f))
    };
    
    Model bunny = Model("Models/Bunny/Bunny2.fbx");
    bunny.Scale(glm::vec3(15.0f));

    Model cube = Model("Models/Cube/Cube.fbx");
    //cube.Rotate(45, glm::vec3(1, 0, 0));
    //cube.Translate(glm::vec3(0, 4, 0));

    models = { bunny, cube};

    GenerateSSBOs();
}

