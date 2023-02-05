#pragma once
#include "ShaderProgram.h"
 
class Scene{
public:
	virtual void SetMathSpheres(ShaderProgram rayTraceShader) = 0;
};

