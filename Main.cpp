#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

struct sphere
{
	cv::Vec3f center;
	float radius;
};

struct ray
{
	cv::Vec3f origin;
	cv::Vec3f direction;
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
		return {};
	}

	return {};
}

int main()
{
	constexpr int screen_width = 400;
	constexpr int screen_height = 400;

	cv::Mat img = cv::Mat(screen_width, screen_height, CV_8UC3);

	const sphere s1 = { { screen_width/2.0f, screen_height/2.0f, 0.0f }, 100.0f };

	for(int y = 0; y < screen_height; ++y)
	{
		for(int x = 0; x < screen_width; ++x)
		{
			ray r1 = { { static_cast<float>(x), static_cast<float>(y), 0 }, { 0, 0, 1 } };

			if(const auto distance = ray_intersect_sphere(r1, s1); distance >= 0)
			{
				uchar color = static_cast<uchar>(std::min(255.0f, distance.value()));

				img.at<cv::Vec3b>(x, y) = { color, color, color };
			}
			else
			{
				img.at<cv::Vec3b>(x, y) = { 255, 0, 0 };
			}

			
		}
	}


	cv::imwrite("sphere.png", img);
	cv::imshow("Sphere", img);
	cv::waitKey();
	return 0;
}