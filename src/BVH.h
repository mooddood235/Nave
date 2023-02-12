#pragma once
#include <vector>
#include "BVHNode.h"
#include "SceneData.h"
#include "Vertex.h"
#include "AABB.h"

class BVH{
public:
	BVH(SceneData sceneData);
	std::vector<BVHNode> GetNodes();
private:
	std::vector<BVHNode> nodes;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int BuildBVH(BVHNode* leafNodes, unsigned int start, unsigned int end, AABB& AABBOut);

	static bool ComparatorX(BVHNode node0, BVHNode node1);
	static bool ComparatorY(BVHNode node0, BVHNode node1);
	static bool ComparatorZ(BVHNode node0, BVHNode node1);
};

