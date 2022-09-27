#include <iostream>
#include <optional>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "BoundingBox.h"
#include "SimpleObject.h"
#include "Tree.h"

std::optional<float> ray_intersect_plan(const ray& ray, const plan& plan) 
{
	float t;

	float denom = plan.normal.dot(ray.direction);
	if (denom > 0) {
		cv::Vec3f dirPlan = plan.position - ray.origin;
		t = dirPlan.dot(plan.normal) / denom;
		if (t >= 0) return t;
	}

	return {};
}

bool ray_intersect_triangle(const ray& ray, const triangle& triangle)
{
	//Ax + By + Cz + D = 0
	//with (A, B, C) normal
	//D = -(Ax + By + Cz)
	float D = triangle.normal.dot(triangle.v0);
	float t = -(triangle.normal.dot(ray.origin) + D) / triangle.normal.dot(ray.direction);
	if (t < 0) return false;

	cv::Vec3f triangle_intersection = ray.origin + t * ray.direction;
	cv::Vec3f C;

	//edge0
	cv::Vec3f edge0 = triangle.v1 - triangle.v0;
	cv::Vec3f vp0 = triangle_intersection - triangle.v0;
	C = edge0.cross(vp0);
	if (triangle.normal.dot(C) < 0) return false;

	//edge 1
	cv::Vec3f edge1 = triangle.v2 - triangle.v1;
	cv::Vec3f vp1 = triangle_intersection - triangle.v1;
	C = edge1.cross(vp1);
	if (triangle.normal.dot(C) < 0) return false;

	//edge 2
	cv::Vec3f edge2 = triangle.v0 - triangle.v2;
	cv::Vec3f vp2 = triangle_intersection - triangle.v2;
	C = edge2.cross(vp2);
	if (triangle.normal.dot(C) < 0) return false;

	return true;
}

std::optional<float> ray_intersect_sphere(const ray& ray, const sphere& sphere)
{
	const cv::Vec3f oc = sphere.center - ray.origin;
	const float r2 = pow(sphere.radius, 2.0f);

	const float a = ray.direction.dot(ray.direction);
	const float b = -2 * ray.direction.dot(oc);
	const float c = oc.dot(oc) - r2;

	const float delta = pow(b, 2.0f) - 4 * a * c;
	const float t0 = (-b - sqrt(delta)) / (2 * a);
	const float t1 = (-b + sqrt(delta)) / (2 * a);

	if(delta >= 0)
	{
		if (t0 >= 0) return t0;
		if (t1 >= 0) return t1;
	}

	return {};
}

std::optional<float> ray_intersect_objects(const ray& ray, const std::vector<sphere>& spheres) {
	std::optional<float> min = {};
	
	for (auto& sphere : spheres) {
		std::optional<float> dist = ray_intersect_sphere(ray, sphere);
	
		if (dist.has_value()) {
			if (min.has_value()) {
				if (dist.value() < min.value())
					min = dist.value();
			}
			else
				min = dist.value();
		}
	}

	return min;
}

int main()
{
	const int screen_width = 400;
	const int screen_height = 400;

	cv::Vec3f offsetCam = { screen_width / 2, screen_height / 2, -150 };

	cv::Mat img = cv::Mat(screen_width, screen_height, CV_8UC3);
	img = cv::Scalar(255, 0, 255);
	
	/*
	std::vector<sphere> spheres = {
		{ { screen_width / 2.0f - 150.0f, screen_height / 2.0f, 200.0f }, 100.0f, { 1, 1, 0 } }, // Back Wall
		{ { screen_width / 2.0f + 50.0f, screen_height / 2.0f, 300.0f }, 100.0f, { 0, 0, 1 }}, // Right Wall
		
	};
	*/

	std::vector<sphere> spheres;
	int sphere_radius = 40;

	const light l1 = { { screen_width / 2.0f + (screen_width / sphere_radius) * sphere_radius / 2, screen_height / 2.0f + (screen_height / sphere_radius)*sphere_radius/2, 150.0f }, { 1, 1, 1 }, 200.0f };

	int i = 0;
	for (int y = 0; y < screen_height / sphere_radius; ++y)
	{
		for (int x = 0; x < screen_width / sphere_radius; ++x)
		{
			spheres.push_back({ {static_cast<float>(x) * sphere_radius * 2, static_cast<float>(y) * sphere_radius * 2, 300.0f}, static_cast<float>(sphere_radius), {1, 1, 1}});
		}
	}

	for(int y = 0; y < screen_height; ++y)
	{
		for(int x = 0; x < screen_width; ++x)
		{
			cv::Vec3f pixelToSee = { static_cast<float>(x), static_cast<float>(y), 0 };
			cv::Vec3f dir = cv::normalize(pixelToSee - offsetCam);
			ray r1 = { pixelToSee, dir };

			/*
			if (box1.intersect(r1)) {
				img.at<cv::Vec3b>(y, x) = { 0, 255, 255 };
			}*/
			
			

			
			//Intersection spheres
			
			for (auto& sphere : spheres) {
				if (const auto distance = ray_intersect_sphere(r1, sphere); distance.has_value())
				{
					cv::Vec3f sphere_intersection = r1.origin + distance.value() * r1.direction;
					cv::Vec3f direction_light = l1.position - sphere_intersection;
					cv::Vec3f normal = cv::normalize(sphere_intersection - sphere.center);

					cv::Vec3f direction_light_normalized = cv::normalize(direction_light);

					const float light_distance2 = direction_light.dot(direction_light);
					const float coef = direction_light.dot(normal) / light_distance2;

					//Calculate Shadow
					cv::Vec3f visibility = { 1, 1, 1 };
					
					const float offset = 0.01f;
					ray ray_shadow_sphere = { sphere_intersection + offset * direction_light_normalized, direction_light_normalized};
					
					if(const auto shadow_dist = ray_intersect_objects(ray_shadow_sphere, spheres); shadow_dist.has_value())
					{
						if (static_cast<float>(pow(shadow_dist.value(), 2)) <= light_distance2) visibility = { 0, 0, 0 };
						else visibility = { 1, 1, 1 };
					}

					//Calculate pixel of image
					img.at<cv::Vec3b>(y, x) = visibility.mul((l1.color * l1.intensity).mul((coef * sphere.diffuse) * 255));
				}

			}

		}
	}


	cv::imwrite("sphere.png", img);
	cv::imshow("Sphere", img);
	cv::waitKey();
	return 0;
}