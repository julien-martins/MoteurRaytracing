#pragma once

#include "BoundingBox.h";
#include "SimpleObject.h"

typedef struct NodeTree {
	NodeTree() {
	}

	NodeTree(std::vector<sphere>& spheres, BoundingBox& box) {
		this->spheres = spheres;
		this->box = box;
		this->left = NULL;
		this->right = NULL;
	}

	BoundingBox box;
	std::vector<sphere> spheres;

	std::unique_ptr<NodeTree> left;
	std::unique_ptr<NodeTree> right;

	BoundingBox& createBoxAroundSpheres(std::vector<sphere>& spheres, std::vector<sphere>& sphere_out) {
		cv::Vec3f min = spheres[0].center;
		cv::Vec3f max = spheres[0].center;

		sphere_out.push_back(spheres[0]);

		for (auto& sphere : spheres) {
			if (sphere.center[0] <= min[0] && sphere.center[1] <= min[1] && sphere.center[2] <= min[2]) {
				min = sphere.center;
				sphere_out.push_back(sphere);
			}

			if (sphere.center[0] >= max[0] && sphere.center[1] >= max[1] && sphere.center[2] >= max[2]) {
				max = sphere.center;
				sphere_out.push_back(sphere);
			}

		}

		BoundingBox box = { min, max };
		return box;
	}

	void splitNode(NodeTree& node, NodeTree& left, NodeTree& right) {

	}

	std::unique_ptr<NodeTree> buildTree(std::vector<sphere> spheres) {
		std::unique_ptr<NodeTree> root;

		if (root->spheres.size() < 0) {
			return NULL;
		}

		//Divide the box in two with all spheres
		root->box = createBoxAroundSpheres(spheres, root->spheres);

		//root->left = buildTree();
		//root->right = buildTree();

		return root;
	}

};