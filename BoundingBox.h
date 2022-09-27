#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "SimpleObject.h"

class BoundingBox {
	public:
		cv::Vec3f pMin;
		cv::Vec3f pMax;

	public:
		BoundingBox() = default;

		BoundingBox(const cv::Vec3f& pMin, const cv::Vec3f& pMax) {
			this->pMin = pMin;
			this->pMax = pMax;
		}

		BoundingBox(const BoundingBox& obj) {
			this->pMin = obj.pMin;
			this->pMax = obj.pMax;
		}

		~BoundingBox() {

		}

		BoundingBox& operator=(const BoundingBox& obj) {
			this->pMin = obj.pMin;
			this->pMax = obj.pMax;
		}
		
		bool intersect(ray& ray) const {
			float tmin, tmax, tymin, tymax, tzmin, tzmax;

			cv::Vec3f invdir = cv::Vec3f(1 / ray.direction[0], 1 / ray.direction[1], 1 / ray.direction[2]);

			if (invdir[0] >= 0) {
				tmin = (this->pMin[0] - ray.origin[0]) * invdir[0];
				tmax = (this->pMax[0] - ray.origin[0]) * invdir[0];
			}
			else {
				tmin = (this->pMax[0] - ray.origin[0]) * invdir[0];
				tmax = (this->pMin[0] - ray.origin[0]) * invdir[0];
			}

			if (invdir[1] >= 0) {
				tymin = (this->pMin[1] - ray.origin[1]) * invdir[1];
				tymax = (this->pMax[1] - ray.origin[1]) * invdir[1];
			}
			else {
				tymin = (this->pMax[1] - ray.origin[1]) * invdir[1];
				tymax = (this->pMin[1] - ray.origin[1]) * invdir[1];
			}

			if ((tmin > tymax) || (tymin > tmax)) return false;

			if (tymin > tmin) tmin = tymin;
			if (tymax < tmax) tmax = tymax;

			if (invdir[1] >= 0) {
				tzmin = (this->pMin[2] - ray.origin[2]) * invdir[2];
				tzmax = (this->pMax[2] - ray.origin[2]) * invdir[2];
			}
			else {
				tzmax = (this->pMin[2] - ray.origin[2]) * invdir[2];
				tzmin = (this->pMax[2] - ray.origin[2]) * invdir[2];
			}

			if ((tmin > tzmax) || (tzmin > tmax)) return false;

			if (tzmin > tmin) tmin = tzmin;
			if (tzmax < tmax) tmax = tzmax;

			return true;
		}

		bool contains(sphere& sphere) const {
			return sphere.center[0] > pMin[0] && sphere.center[1] > pMin[1] && sphere.center[2] > pMin[2]
				&& sphere.center[0] < pMax[0] && sphere.center[1] < pMax[1] && sphere.center[3] < pMax[3];
		}

};