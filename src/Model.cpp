#include "Model.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include<assimp/postprocess.h>
#include <vector>

#include "Vertex.h"
#include "TextureMaterial.h"
#include <stb_image/stb_image.h>

Model::Model(std::string modelPath) {
	modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

	const unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs;

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
		if (!mesh->mTextureCoords[0]) {
			std::cout << "ERROR: A model with no UVs has been imported." << std::endl;
			exit(-1);
		}

		Vertex vertex = Vertex(
			aiVector3DToGLMVec3(mesh->mVertices[i]),
			aiVector3DToGLMVec3(mesh->mNormals[i]),
			aiVector3DToGLMVec3(mesh->mTangents[i]),
			aiVector3DToGLMVec3(mesh->mBitangents[i]),
			glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
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
		textureMaterial.albedoTexture = LoadTexture(material, aiTextureType_DIFFUSE);
		textureMaterial.roughnessTexture = LoadTexture(material, aiTextureType_SHININESS);
		textureMaterial.metalnessTexture = LoadTexture(material, aiTextureType_METALNESS);
		textureMaterial.emissionTexture = LoadTexture(material, aiTextureType_EMISSION_COLOR);
		textureMaterial.normalsTexture = LoadTexture(material, aiTextureType_NORMALS);

		aiColor3D albedo = aiColor3D(-1.0f, -1.0f, -1.0f);
		aiColor3D emission = aiColor3D(-1.0, -1.0, -1.0f);
		float roughness = -1.0f;
		float metalness = -1.0f;

		if (textureMaterial.albedoTexture.path.compare("src/Textures/DefaultTexture.png") == 0)	material->Get(AI_MATKEY_COLOR_DIFFUSE, albedo);
		if (textureMaterial.emissionTexture.path.compare("src/Textures/DefaultTexture.png") == 0) material->Get(AI_MATKEY_COLOR_EMISSIVE, emission);
		if (textureMaterial.roughnessTexture.path.compare("src/Textures/DefaultTexture.png") == 0) material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
		if (textureMaterial.roughnessTexture.path.compare("src/Textures/DefaultTexture.png") == 0) material->Get(AI_MATKEY_REFLECTIVITY, metalness);

		textureMaterial.albedo = aiColor3DToGLMVec3(albedo);
		textureMaterial.emission = aiColor3DToGLMVec3(emission);
		textureMaterial.roughness = roughness;
		textureMaterial.metalness = metalness;
	}
	return Mesh(vertices, indices, textureMaterial);
}
Texture Model::LoadTexture(aiMaterial* material, aiTextureType type) {
	if (material->GetTextureCount(type) == 0) {
		if (type == aiTextureType_NORMALS) return Texture("src/Textures/DefaultNormals.png", type);
		return Texture("src/Textures/DefaultTexture.png", type);
	}

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
glm::vec3 Model::aiColor3DToGLMVec3(aiColor3D color) {
	return glm::vec3(color.r, color.g, color.b);
}
std::vector<Mesh> Model::GetMeshes() {
	return meshes;
}