#pragma once
#include "ShaderProgram.h"
#include "SceneData.h"
#include "Model.h"
#include "MathSphere.h"

class Scene{
public:
	void SetMathSpheres(ShaderProgram rayTraceShader);
	void SetMeshes(ShaderProgram rayTraceShader);

	SceneData GetData();
protected:
	void GenerateSSBOs();

	std::vector<MathSphere> mathSpheres;
	std::vector<Model> models;

	unsigned int indexCount = 0;
	unsigned int nodeCount = 0;

	unsigned int verticesSSBO;
	unsigned int indicesSSBO;
	unsigned int BVHSSBO;
};

