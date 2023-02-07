#pragma once
#include "ShaderProgram.h"
#include "SceneData.h"
#include "Model.h"

class Scene{
public:
	virtual void SetMathSpheres(ShaderProgram rayTraceShader) = 0;
	virtual void SetMeshes(ShaderProgram rayTraceShader) = 0;

	static SceneData GetData(std::vector<Model> models);
};

