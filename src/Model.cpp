#include "Model.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include<assimp/postprocess.h>
#include <vector>

#include "Vertex.h"


Model::Model(std::string modelPath) {
	this->modelPath = modelPath;

	const unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Assimp Error: " << importer.GetErrorString() << std::endl;
		glfwTerminate();
		exit(-1);
	}
	GetMeshes(scene->mRootNode, scene);
}
void Model::Draw() {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw();
	}
}
void Model::GetMeshes(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(aiMeshToMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		aiNode* childNode = node->mChildren[i];
		GetMeshes(childNode, scene);
	}
}
Mesh Model::aiMeshToMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex = Vertex(aiVector3DToGLMVec3(mesh->mVertices[i]), aiVector3DToGLMVec3(mesh->mNormals[i]));
		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			unsigned int index = face.mIndices[j];
			indices.push_back(index);
		}
	}
	return Mesh(vertices, indices);
}
glm::vec3 Model::aiVector3DToGLMVec3(aiVector3D vector) {
	return glm::vec3(vector.x, vector.y, vector.z);
}