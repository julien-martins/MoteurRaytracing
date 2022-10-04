#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

struct sphere
{
	cv::Vec3f center;
	float radius;

	cv::Vec3f diffuse;
	std::string Mat;
};

struct plan
{
	cv::Vec3f position;
	cv::Vec3f normal;
};

struct triangle
{
	cv::Vec3f v0;
	cv::Vec3f v1;
	cv::Vec3f v2;

	cv::Vec3f normal;
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