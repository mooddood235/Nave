#include "Model.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include<assimp/postprocess.h>
#include <vector>

#include "Vertex.h"
#include "TextureMaterial.h"

Model::Model(std::string modelPath) {
	modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

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
	TextureMaterial textureMaterial;

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
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		textureMaterial.diffuseTexture = LoadTexture(material, aiTextureType_DIFFUSE);
	}
	return Mesh(vertices, indices, textureMaterial);
}
Texture Model::LoadTexture(aiMaterial* material, aiTextureType type) {
	if (material->GetTextureCount(type) == 0) return Texture("src/Textures/DefaultTexture.png", aiTextureType_DIFFUSE);

	aiString fileName;
	material->GetTexture(type, 0, &fileName);

	std::string filePath = modelDirectory + "/" + fileName.C_Str();

	for (unsigned int i = 0; i < loadedTextures.size(); i++){
		if (loadedTextures[i].path.compare(filePath) == 0) {
			return loadedTextures[i];
		}
	}

	Texture newTexture = Texture(filePath, type);
	loadedTextures.push_back(newTexture);

	return newTexture;
}
glm::vec3 Model::aiVector3DToGLMVec3(aiVector3D vector) {
	return glm::vec3(vector.x, vector.y, vector.z);
}
std::vector<Mesh> Model::GetMeshes() {
	return meshes;
}