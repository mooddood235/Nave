#include "DefaultScene.h"
#include "../src/SceneData.h"

DefaultScene::DefaultScene() {
    cube.Translate(glm::vec3(2, 5, 2));
}

void DefaultScene::SetMathSpheres(ShaderProgram rayTraceShader) {
	for (unsigned int i = 0; i < sizeof(mathSpheres) / sizeof(MathSphere); i++) {
        MathSphere mathSphere = mathSpheres[i];
        std::string mathSphereString = "mathSpheres[" + std::to_string(i) + "]";

        rayTraceShader.SetVec3(mathSphereString + ".position", mathSphere.GetPosition());
        rayTraceShader.SetFloat(mathSphereString + ".radius", mathSphere.GetRadius());
        rayTraceShader.SetVec3(mathSphereString + ".albedo", mathSphere.material.albedo);
        rayTraceShader.SetFloat(mathSphereString + ".roughness", mathSphere.material.roughness);
        rayTraceShader.SetFloat(mathSphereString + ".metalness", mathSphere.material.metalness);
	}
    rayTraceShader.SetUnsignedInt("mathSphereCount", sizeof(mathSpheres) / sizeof(MathSphere));
}
void DefaultScene::SetMeshes(ShaderProgram rayTraceShader) {
    rayTraceShader.BindStorageBlock("Vertices", 0);
    rayTraceShader.BindStorageBlock("Indices", 1);

    SceneData sceneData = Scene::GetData(std::vector<Model>{cube});

    unsigned int ssboVertices;
    unsigned int ssboIndices;

    glGenBuffers(1, &ssboVertices);
    glGenBuffers(1, &ssboIndices);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboVertices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboIndices);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboVertices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sceneData.vertices.size(), NULL, GL_STATIC_DRAW);
    
    for (unsigned int i = 0; i < sceneData.vertices.size(); i++) {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 16, sizeof(glm::vec3), &(sceneData.vertices[i].pos));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboIndices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sceneData.indices.size(), NULL, GL_STATIC_DRAW);
    
    for (unsigned int i = 0; i < sceneData.indices.size(); i++) {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 16, 4, &(sceneData.indices[i]));
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    rayTraceShader.SetUnsignedInt("indexCount", sceneData.indices.size());
}
