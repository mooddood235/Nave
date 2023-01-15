#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>


class ShaderProgram{
public:
	ShaderProgram(std::string vertPath, std::string fragPath);
	~ShaderProgram();
	void Use();
	static void Unuse();
	void SetMat4(std::string uniformName, glm::mat4 value);
	void SetVec3(std::string uniformName, glm::vec3 value);
	void SetFloat(std::string uniformName, float value);
private:
	unsigned int shaderProgram;

	unsigned int CompileShader(std::string path, GLenum type);
	unsigned int LinkProgram(unsigned int vertShader, unsigned int fragShader);
	void ShaderCompilationErrorCheck(unsigned int shader, std::string path);
	void ProgramLinkingErrorCheck(unsigned int shaderProgram);
};

