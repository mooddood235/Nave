#include "ShaderProgram.h"
#include <sstream>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(std::string vertPath, std::string fragPath) {
	shaderProgram = LinkProgram(CompileShader(vertPath, GL_VERTEX_SHADER), CompileShader(fragPath, GL_FRAGMENT_SHADER));
}
void ShaderProgram::Use() {
	glUseProgram(shaderProgram);
}
void ShaderProgram::Unuse() {
	glUseProgram(0);
}
void ShaderProgram::SetMat4(std::string uniformName, glm::mat4 value) {
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glProgramUniformMatrix4fv(shaderProgram, location, 1, GL_FALSE, glm::value_ptr(value));
}
void ShaderProgram::SetVec3(std::string uniformName, glm::vec3 value) {
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glProgramUniform3fv(shaderProgram, location, 1, glm::value_ptr(value));
}
void ShaderProgram::SetFloat(std::string uniformName, float value) {
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glProgramUniform1f(shaderProgram, location, value);
}
void ShaderProgram::SetDouble(std::string uniformName, double value) {
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glProgramUniform1d(shaderProgram, location, value);
}
void ShaderProgram::SetInt(std::string uniformName, int value) {
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glProgramUniform1i(shaderProgram, location, value);
}
void ShaderProgram::SetUnsignedInt(std::string uniformName, unsigned int value) {
	unsigned int location = glGetUniformLocation(shaderProgram, uniformName.c_str());
	glProgramUniform1ui(shaderProgram, location, value);
}
unsigned int ShaderProgram::CompileShader(std::string path, GLenum type) {
	if (!(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER)) {
		std::cout << "ERROR: Cannot compile shader of type <" << std::to_string(type) << ">" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	std::ifstream file = std::ifstream(path);
	std::stringstream stringstream;

	if (!file.is_open()) {
		std::cout << "ERROR: Could not open file at path <" << path << ">" << std::endl;
		file.close();
		glfwTerminate();
		exit(-1);
	}
	stringstream << file.rdbuf();
	std::string shaderContents = stringstream.str();

	unsigned int shader = glCreateShader(type);
	const char* shaderContentsCString = shaderContents.c_str(); // glShaderSource() requires a const double pointer thingy.
	glShaderSource(shader, 1, &shaderContentsCString, NULL);
	glCompileShader(shader);
	ShaderCompilationErrorCheck(shader, path);

	file.close();

	return shader;
}
unsigned int ShaderProgram::LinkProgram(unsigned int vertShader, unsigned int fragShader) {
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	ProgramLinkingErrorCheck(shaderProgram);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}
void ShaderProgram::ShaderCompilationErrorCheck(unsigned int shader, std::string path) {
	int success;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);

		std::cout << "Error: Could not compile shader at path <" << path << ">. Error message:\n" << infoLog << std::endl;
		glfwTerminate();
		exit(-1);
	}
}
void ShaderProgram::ProgramLinkingErrorCheck(unsigned int shaderProgram) {
	int success;

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);

		std::cout << "Error: Could not link shader program. Error message:\n" << infoLog << std::endl;
		glfwTerminate();
		exit(-1);
	}
}