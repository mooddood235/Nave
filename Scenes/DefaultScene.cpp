#include "DefaultScene.h"
#include "../src/SceneData.h"

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
    rayTraceShader.BindUniformBlock("Vertices", 0);
    rayTraceShader.BindUniformBlock("Indices", 1);

    SceneData sceneData = Scene::GetData(std::vector<Model>{cube});

    unsigned int uboVertices;
    unsigned int uboIndices;

    glGenBuffers(1, &uboVertices);
    glGenBuffers(1, &uboIndices);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboVertices);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboIndices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboVertices);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sceneData.vertices.size(), NULL, GL_STATIC_DRAW);
    
    for (unsigned int i = 0; i < sceneData.vertices.size(); i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, i * 16, sizeof(glm::vec3), &(sceneData.vertices[i].pos));
    }
    glBindBuffer(GL_UNIFORM_BUFFER, uboIndices);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sceneData.indices.size(), NULL, GL_STATIC_DRAW);
    
    for (unsigned int i = 0; i < sceneData.indices.size(); i++) {
        glBufferSubData(GL_UNIFORM_BUFFER, i * 16, 4, &(sceneData.indices[i]));
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    rayTraceShader.SetUnsignedInt("indexCount", sceneData.indices.size());

    for (unsigned int i = 0; i < sceneData.vertices.size(); i++) {
        rayTraceShader.SetVec3("_vertices[" + std::to_string(i) + "]", sceneData.vertices[i].pos);
    }
    for (unsigned int i = 0; i < sceneData.indices.size(); i++) {
        rayTraceShader.SetUnsignedInt("_indices[" + std::to_string(i) + "]", sceneData.indices[i]);
    }

}
