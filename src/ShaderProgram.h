#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>


class ShaderProgram{
public:
	ShaderProgram(std::string vertPath, std::string fragPath);
	void Use();
	static void Unuse();
	void SetMat4(std::string uniformName, glm::mat4 value);
	void SetVec3(std::string uniformName, glm::vec3 value);
	void SetFloat(std::string uniformName, float value);
	void SetDouble(std::string uniformName, double value);
	void SetInt(std::string uniformName, int value);
	void SetUnsignedInt(std::string uniformName, unsigned int value);
	void BindUniformBlock(std::string blockName, unsigned int bind);
	void BindStorageBlock(std::string blockName, unsigned int bind);
private:
	unsigned int shaderProgram;

	unsigned int CompileShader(std::string path, GLenum type);
	unsigned int LinkProgram(unsigned int vertShader, unsigned int fragShader);
	void ShaderCompilationErrorCheck(unsigned int shader, std::string path);
	void ProgramLinkingErrorCheck(unsigned int shaderProgram);
};

