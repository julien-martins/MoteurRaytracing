#pragma once

#include "BoundingBox.h";
#include "SimpleObject.h"

struct nodeTree {
	BoundingBox box;
	std::vector<sphere> spheres;
};

class Tree {
	public:
		Tree() {

		}

		std::vector<nodeTree> nodes;

		BoundingBox& createBoxAroundSpheres(std::vector<sphere>& spheres) {
			cv::Vec3f min = spheres.at(0).center;
			cv::Vec3f max = spheres.at(0).center;

			for (auto& sphere : spheres) {
				if (sphere.center[0] <= min[0] && sphere.center[1] <= min[1] && sphere.center[2] <= min[2])
					min = sphere.center;

				if (sphere.center[0] >= max[0] && sphere.center[1] >= max[1] && sphere.center[2] >= max[2])
					max = sphere.center;
			}
			BoundingBox box = { min, max };
			return box;
		}
};