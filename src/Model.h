#pragma once
#include <string>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "GameObject.h"
#include "Texture.h"

class Model : public GameObject{
public:
	Model(std::string modelPath);
	void Draw();
	std::vector<Mesh> GetMeshes();
private:
	std::string modelDirectory;
	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;

	void GetMeshes(aiNode* node, const aiScene* scene);
	Mesh aiMeshToMesh(aiMesh* mesh, const aiScene* scene);
	glm::vec3 aiVector3DToGLMVec3(aiVector3D vector);
	glm::vec3 aiColor3DToGLMVec3(aiColor3D color);
	Texture LoadTexture(aiMaterial* material, aiTextureType type);
};

