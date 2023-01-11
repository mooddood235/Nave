#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>


class ShaderProgram{
public:
	ShaderProgram(std::string vertPath, std::string fragPath);
	~ShaderProgram();
private:
	unsigned int shaderProgram;

	unsigned int CompileShader(std::string path, GLenum type);
	unsigned int LinkProgram(unsigned int vertShader, unsigned int fragShader);
	void ShaderCompilationErrorCheck(unsigned int shader, std::string path);
	void ProgramLinkingErrorCheck(unsigned int shaderProgram);
};

