#include "EnvironmentMap.h"
#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "WindowInfo.h"



EnvironmentMap::EnvironmentMap(std::string path) {
	LoadEnvironmentMap(path);
	//GenerateCubeMap();
}
unsigned int EnvironmentMap::GetEnvironmentMap() {
	return environmentMap;
}
void EnvironmentMap::LoadEnvironmentMap(std::string path) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, numChannels;
	float* data = stbi_loadf(path.c_str(), &width, &height, &numChannels, 0);

	if (!data) {
		std::cout << "ERROR: Could not load enviroment map with path <" << path << ">" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glGenTextures(1, &environmentMap);
	glBindTexture(GL_TEXTURE_2D, environmentMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_set_flip_vertically_on_load(false);
}
