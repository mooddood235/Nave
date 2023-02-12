#include "BVH.h"
#include <algorithm>
#include <cmath>
#include <iostream>

BVH::BVH(SceneData sceneData) {
	vertices = sceneData.vertices;
	indices = sceneData.indices;

	BVHNode* leafNodes = (BVHNode*)malloc(sizeof(BVHNode) * indices.size() / 3);

	for (unsigned int i = 0; i < indices.size() / 3; i++) {
		glm::vec3 v0 = vertices[indices[i * 3]].pos;
		glm::vec3 v1 = vertices[indices[i * 3 + 1]].pos;
		glm::vec3 v2 = vertices[indices[i * 3 + 2]].pos;

		leafNodes[i] = BVHNode(AABB(v0, v1, v2), true, i * 3, 0);
	}
	AABB BVHAABB;
	BuildBVH(leafNodes, 0, indices.size() / 3 - 1, BVHAABB);

	free(leafNodes);
}

unsigned int BVH::BuildBVH(BVHNode* leafNodes, unsigned int start, unsigned int end, AABB& AABBOut) {
	if (start == end) {
		nodes.push_back(leafNodes[start]);
		AABBOut = leafNodes[start].aabb;
		return nodes.size() - 1;
	}
	nodes.push_back(BVHNode());
	unsigned int thisNodeIndex = nodes.size() - 1;

	bool (*comparators[])(BVHNode, BVHNode) { ComparatorX, ComparatorY, ComparatorZ };

	std::sort(leafNodes + start, leafNodes + end, comparators[rand() % 3]);

	AABB leftAABB;
	AABB rightAABB;
	
	unsigned int middle = start + (end - start) / 2;

	unsigned int leftIndex = BuildBVH(leafNodes, start, middle, leftAABB);
	unsigned int rightIndex = BuildBVH(leafNodes, middle + 1, end, rightAABB);

	AABBOut = AABB(leftAABB, rightAABB);

	nodes[thisNodeIndex] = BVHNode(AABBOut, false, leftIndex, rightIndex);

	return thisNodeIndex;
}
std::vector<BVHNode> BVH::GetNodes() {
	return nodes;
}
bool BVH::ComparatorX(BVHNode node0, BVHNode node1) {
	return node0.aabb.cornerMin.x <= node1.aabb.cornerMin.x;
}
bool BVH::ComparatorY(BVHNode node0, BVHNode node1) {
	return node0.aabb.cornerMin.y <= node1.aabb.cornerMin.y;
}
bool BVH::ComparatorZ(BVHNode node0, BVHNode node1) {
	return node0.aabb.cornerMin.z <= node1.aabb.cornerMin.z;
}
