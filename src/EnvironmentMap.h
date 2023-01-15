#pragma once
#include <string>

#include "ShaderProgram.h"
#include "Model.h"

struct EnvironmentMap{
public:
	EnvironmentMap(std::string path);
	unsigned int GetEnvironmentMap();
	unsigned int GetEnvironmentCubeMap();
private:
	unsigned int environmentMap;
	unsigned int environmentCubeMap;

	void LoadEnvironmentMap(std::string path);
	void GenerateCubeMap();
};

