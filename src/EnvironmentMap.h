#pragma once
#include <string>

#include "ShaderProgram.h"
#include "Model.h"

struct EnvironmentMap{
public:
	EnvironmentMap(std::string path);
	unsigned int GetEnvironmentMap();
private:
	unsigned int environmentMap;

	void LoadEnvironmentMap(std::string path);
};

