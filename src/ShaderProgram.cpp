#include "ShaderProgram.h"
#include <sstream>
#include <GLFW/glfw3.h>

ShaderProgram::ShaderProgram(std::string vertPath, std::string fragPath) {
	shaderProgram = LinkProgram(CompileShader(vertPath, GL_VERTEX_SHADER), CompileShader(fragPath, GL_FRAGMENT_SHADER));
}
ShaderProgram::~ShaderProgram() {
	glDeleteProgram(shaderProgram);
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