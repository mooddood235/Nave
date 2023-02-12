#include "BVHNode.h"

BVHNode::BVHNode() {
	aabb = AABB();
	isLeaf = false;
	left = 0;
	right = 0;
}

BVHNode::BVHNode(AABB aabb, bool isLeaf, unsigned int left, unsigned int right) {
	this->aabb = aabb;
	this->isLeaf = isLeaf;
	this->left = left;
	this->right = right;
}