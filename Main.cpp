#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

struct sphere
{
	cv::Vec3f center;
	float radius;

	cv::Vec3f diffuse;
};

struct ray
{
	cv::Vec3f origin;
	cv::Vec3f direction;
};

struct light
{
	cv::Vec3f position;
	cv::Vec3f color;
	float intensity;
};

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

int main()
{
	constexpr int screen_width = 400;
	constexpr int screen_height = 400;

	cv::Mat img = cv::Mat(screen_width, screen_height, CV_8UC3);
	img = cv::Scalar(255, 0, 255);
	
	const light l1 = { { 300.0f, screen_height / 2.0f, 50.0f }, { 1, 0, 0 }, 300.0f };

	std::vector<sphere> spheres = {
									{ { screen_width / 2.0f, screen_height / 2.0f, 3000 }, 340.0f, { 1, 1, 0 } }, // Back Wall
									{ { 3000.0f + 340.0f, screen_height / 2.0f, 0.0f }, 3000.0f, { 0, 0, 1 }}, // Right Wall
									{ { -3000.0f + 40.0f, screen_height / 2.0f, 0.0f }, 3000.0f, { 0, 0, 1 }}, // Left Wall
									{ { screen_width / 2.0f, -3000.0f + 40.0f, 0.0f }, 3000.0f, { 1, 0, 0 } }, // Top Wall
									{ { screen_width / 2.0f, 3000.0f + 300.0f, 0.0f }, 3000.0f, { 1, 0, 0 } }, // Bottom Wall
									
									{ { 100.0f, screen_height / 2.0f, 100.0f }, 50.0f, { 1, 1, 1 } } // Sphere 1
									};

	for(int y = 0; y < screen_height; ++y)
	{
		for(int x = 0; x < screen_width; ++x)
		{
			ray r1 = { { static_cast<float>(x), static_cast<float>(y), 0 }, { 0, 0, 1 } };

			for (auto it = spheres.begin(); it != spheres.end(); ++it) {

				if (const auto distance = ray_intersect_sphere(r1, *it); distance >= 0)
				{
					cv::Vec3f sphere_intersection = r1.origin + distance.value() * r1.direction;
					cv::Vec3f direction_light = l1.position - sphere_intersection;
					cv::Vec3f normal = cv::normalize(sphere_intersection - (*it).center);

					float lightDistance2 = direction_light.dot(direction_light);
					float coef = direction_light.dot(normal) / lightDistance2;

					img.at<cv::Vec3b>(y, x) = (l1.color * l1.intensity).mul((coef * (*it).diffuse) * 255);
				}

			}
			
		}
	}


	cv::imwrite("sphere.png", img);
	cv::imshow("Sphere", img);
	cv::waitKey();
	return 0;
}