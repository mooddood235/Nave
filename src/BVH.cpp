#include "BVH.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <glad/glad.h>

BVH::BVH(std::vector<Model> models) {
	std::vector<BVHNode> leafNodes;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureMaterial> textureMaterials;
	std::vector<BVHNode> nodes;

	for (unsigned int i = 0; i < models.size(); i++) {
		Model model = models[i];
		std::vector<Mesh> meshes = model.GetMeshes();

		for (unsigned int j = 0; j < meshes.size(); j++) {
			Mesh mesh = meshes[j];
			
			textureMaterials.push_back(mesh.GetTextureMaterial());

			std::vector<Vertex> meshVertices = mesh.GetVertices();
			std::vector<unsigned int> meshIndices = mesh.GetIndices();

			unsigned int indexOffset = vertices.size();

			for (unsigned int k = 0; k < meshVertices.size(); k++) {
				meshVertices[k].pos = model.GetModelMatrix() * glm::vec4(meshVertices[k].pos, 1);
				meshVertices[k].normal = glm::normalize(model.GetNormalMatrix() * meshVertices[k].normal);
				meshVertices[k].tangent = glm::normalize(model.GetNormalMatrix() * meshVertices[k].tangent);
				meshVertices[k].biTangent = glm::normalize(model.GetNormalMatrix() * meshVertices[k].biTangent);

				vertices.push_back(meshVertices[k]);
			}

			for (unsigned int k = 0; k < meshIndices.size() / 3; k++) {
				unsigned int index0 = meshIndices[k * 3];
				unsigned int index1 = meshIndices[k * 3 + 1];
				unsigned int index2 = meshIndices[k * 3 + 2];
				
				indices.push_back(index0 + indexOffset);
				indices.push_back(index1 + indexOffset);
				indices.push_back(index2 + indexOffset);

				glm::vec3 v0 = meshVertices[index0].pos;
				glm::vec3 v1 = meshVertices[index1].pos;
				glm::vec3 v2 = meshVertices[index2].pos;

				leafNodes.push_back(BVHNode(AABB(v0, v1, v2), true, indices.size() - 3, textureMaterials.size() - 1));
			}			
		}
	}
	AABB BVHAABB;
	BuildBVH(nodes, leafNodes.data(), 0, leafNodes.size() - 1, BVHAABB);
	
	indexCount = indices.size();
	nodeCount = nodes.size();
	GenerateSSBOs(vertices, indices, nodes, textureMaterials);
}
unsigned int BVH::BuildBVH(std::vector<BVHNode>& nodes, BVHNode* leafNodes, unsigned int start, unsigned int end, AABB& AABBOut) {
	if (start > end) {
		std::cout << "ERROR: start more than end." << std::endl;
		exit(-1);
	}
	else if (start == end) {
		nodes.push_back(leafNodes[start]);
		AABBOut = leafNodes[start].aabb;
		return nodes.size() - 1;
	}
	nodes.push_back(BVHNode());
	unsigned int thisNodeIndex = nodes.size() - 1;

	bool (*comparators[])(BVHNode, BVHNode) { ComparatorX, ComparatorY, ComparatorZ };

	unsigned int bestSplitIndex = 0;
	unsigned int bestAxis = 0;
	float bestCost = INFINITY;

	for (unsigned int axis = 0; axis <= 2; axis++) {
		std::sort(leafNodes + start, leafNodes + end, comparators[axis]);
		for (unsigned int splitIndex = start; splitIndex <= end; splitIndex++) {
			float splitCost = ComputeSplitCost(leafNodes, start, splitIndex, end);

			if (splitCost < bestCost) {
				bestSplitIndex = splitIndex;
				bestAxis = axis;
				bestCost = splitCost;
			}
		}
	}
	std::sort(leafNodes + start, leafNodes + end, comparators[bestAxis]);

	AABB leftAABB;
	AABB rightAABB;

	//unsigned int middle = start + (end - start) / 2;

	unsigned int leftIndex = BuildBVH(nodes, leafNodes, start, bestSplitIndex, leftAABB);
	unsigned int rightIndex = BuildBVH(nodes, leafNodes, bestSplitIndex + 1, end, rightAABB);

	AABBOut = AABB(leftAABB, rightAABB);

	//std::cout << "CornerMin: " << AABBOut.cornerMin.x << "," << AABBOut.cornerMin.y << "," << AABBOut.cornerMin.z << " CornerMax: " << AABBOut.cornerMax.x << "," << AABBOut.cornerMax.y << "," << AABBOut.cornerMax.z << std::endl;

	nodes[thisNodeIndex] = BVHNode(AABBOut, false, leftIndex, rightIndex);

	return thisNodeIndex;
}
void BVH::GenerateSSBOs(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<BVHNode> nodes, std::vector<TextureMaterial> textureMaterials) {
	glGenBuffers(1, &verticesSSBO);
	glGenBuffers(1, &indicesSSBO);
	glGenBuffers(1, &BVHSSBO);
	glGenBuffers(1, &textureMaterialsSSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 80 * vertices.size(), NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 80, 12, &(vertices[i].pos));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 80 + 16, 12, &(vertices[i].normal));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 80 + 16 * 2, 12, &(vertices[i].tangent));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 80 + 16 * 3, 12, &(vertices[i].biTangent));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 80 + 16 * 4, 8, &(vertices[i].uv));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * indices.size(), NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < indices.size(); i++) {
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 16, 4, &(indices[i]));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, BVHSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, nodes.size() * 48, NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < nodes.size(); i++) {
		unsigned int isLeaf = nodes[i].isLeaf ? 1 : 0;

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48, 4, &isLeaf);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 4, 4, &(nodes[i].left));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 8, 4, &(nodes[i].right));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 16, 12, &(nodes[i].aabb.cornerMin));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 48 + 32, 12, &(nodes[i].aabb.cornerMax));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureMaterialsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, textureMaterials.size() * 96, NULL, GL_STATIC_DRAW);

	for (unsigned int i = 0; i < textureMaterials.size(); i++) {
		TextureMaterial textureMaterial = textureMaterials[i];

		uint64_t handles[] = {
			glGetTextureHandleARB(textureMaterial.albedoTexture.id),
			glGetTextureHandleARB(textureMaterial.roughnessTexture.id),
			glGetTextureHandleARB(textureMaterial.metalnessTexture.id),
			glGetTextureHandleARB(textureMaterial.emissionTexture.id),
			glGetTextureHandleARB(textureMaterial.normalsTexture.id)
		};

		for (unsigned int j = 0; j < 5; j++) {
			if (std::find(textureHandles.begin(), textureHandles.end(), handles[j]) == textureHandles.end()) textureHandles.push_back(handles[j]);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 96 + 8 * j, 8, handles + j);
		}
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 96 + 16 * 3, 12, &textureMaterial.albedo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 96 + 16 * 4, 4, &textureMaterial.roughness);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 96 + 16 * 4 + 4, 4, &textureMaterial.metalness);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 96 + 16 * 5, 12, &textureMaterial.emission);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void BVH::SetSSBOs(ShaderProgram rayTraceShader) {
	rayTraceShader.BindStorageBlock("Vertices", 0);
	rayTraceShader.BindStorageBlock("Indices", 1);
	rayTraceShader.BindStorageBlock("BVH", 2);
	rayTraceShader.BindStorageBlock("TextureMaterials", 3);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, verticesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indicesSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BVHSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, textureMaterialsSSBO);

	rayTraceShader.SetUnsignedInt("indexCount", indexCount);
	rayTraceShader.SetUnsignedInt("nodeCount", nodeCount);
}
void BVH::MakeHandlesResident() {
	for (unsigned int i = 0; i < textureHandles.size(); i++) {
		glMakeTextureHandleResidentARB(textureHandles[i]);
	}
}
void BVH::MakeHandlesNotResident() {
	for (unsigned int i = 0; i < textureHandles.size(); i++) {
		glMakeTextureHandleNonResidentARB(textureHandles[i]);
	}
}
bool BVH::ComparatorX(BVHNode node0, BVHNode node1) {
	return node0.aabb.GetCentroid().x <= node1.aabb.GetCentroid().x;
}
bool BVH::ComparatorY(BVHNode node0, BVHNode node1) {
	return node0.aabb.GetCentroid().y <= node1.aabb.GetCentroid().y;
}
bool BVH::ComparatorZ(BVHNode node0, BVHNode node1) {
	return node0.aabb.GetCentroid().z <= node1.aabb.GetCentroid().z;
}
float BVH::ComputeSplitCost(BVHNode* leafNodes, unsigned int start, unsigned int split, unsigned int end) {
	const float tTraversal = 1.0f;
	const float tIntersect = 2.0f;

	float intersectionCost_A = (split - start + 1) * tIntersect, intersectionCost_B = (end - split) * tIntersect;

	float S_T = 0, S_A = 0, S_B = 0;

	for (unsigned int i = start; i <= split; i++) S_A += leafNodes[i].aabb.GetSurfaceArea();
	for (unsigned int i = split + 1; i <= end; i++) S_B += leafNodes[i].aabb.GetSurfaceArea();
	S_T = S_A + S_B;

	float P_A = S_A / S_T, P_B = S_B / S_T;

	return tTraversal + P_A * intersectionCost_A + P_B * intersectionCost_B;
}
BVH BVH::CameraBVH() {
	Model camera = Model("Models/Camera/Camera.fbx");
	camera.Rotate(-90, glm::vec3(1, 0, 0));
	camera.Rotate(180, glm::vec3(0, 1, 0));
	camera.Scale(glm::vec3(15));
	return BVH({ camera });
}
BVH BVH::CornellBVH() {
	Model cornell = Model("Models/Cornell/Cornell.fbx");
	cornell.Rotate(180, glm::vec3(1, 0, 0));
	return BVH({ cornell });
}
BVH BVH::LanternBVH() {
	Model lantern = Model("Models/Lantern/Lantern.fbx");
	lantern.Rotate(-90, glm::vec3(1, 0, 0), Space::local);
	//lantern.Scale(glm::vec3(8));
	return BVH({ lantern });
}
BVH BVH::FruitsBVH() {
	Model fruits = Model("Models/Fruits/Fruits.fbx");
	fruits.Rotate(-90, glm::vec3(1, 0, 0));

	return BVH({ fruits });
}

