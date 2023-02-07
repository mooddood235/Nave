#pragma once
#include <string>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "GameObject.h"

class Model : public GameObject{
public:
	Model(std::string modelPath);
	void Draw();
	std::vector<Mesh> GetMeshes();
private:
	std::string modelPath;
	std::vector<Mesh> meshes;

	void GetMeshes(aiNode* node, const aiScene* scene);
	Mesh aiMeshToMesh(aiMesh* mesh, const aiScene* scene);
	glm::vec3 aiVector3DToGLMVec3(aiVector3D vector);
};

