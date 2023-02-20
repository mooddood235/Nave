#pragma once
#include <vector>
#include "BVHNode.h"
#include "Vertex.h"
#include "AABB.h"
#include "Model.h"
#include "TextureMaterial.h"
#include "ShaderProgram.h"

class BVH{
public:
	BVH(std::vector<Model> models);
	void SetSSBOs(ShaderProgram rayTraceShader);
	void MakeHandlesResident();
	void MakeHandlesNotResident();

	// Preset BVHs
	static BVH DefaultBVH();
	static BVH CornellBVH();
	static BVH LanternBVH();

private:
	unsigned int verticesSSBO;
	unsigned int indicesSSBO;
	unsigned int BVHSSBO;
	unsigned int textureMaterialsSSBO;

	unsigned int indexCount;
	unsigned int nodeCount;

	std::vector<uint64_t> textureHandles;

	void GenerateSSBOs(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<BVHNode> nodes, std::vector<TextureMaterial> textureMaterials);
	unsigned int BuildBVH(std::vector<BVHNode>& nodes, BVHNode* leafNodes, unsigned int start, unsigned int end, AABB& AABBOut);

	static bool ComparatorX(BVHNode node0, BVHNode node1);
	static bool ComparatorY(BVHNode node0, BVHNode node1);
	static bool ComparatorZ(BVHNode node0, BVHNode node1);
};

