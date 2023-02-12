#pragma once
#include "AABB.h"

struct BVHNode{
	AABB aabb;
	
	bool isLeaf;

	unsigned int left;
	unsigned int right;

	BVHNode();
	BVHNode(AABB aabb, bool isLeaf, unsigned int left, unsigned int right);
};

