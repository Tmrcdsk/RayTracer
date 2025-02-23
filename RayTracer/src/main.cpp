#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Vector.h"

struct Ray
{
	vec3 orig;
	vec3 dir;

	Ray() : orig(0.0f), dir(0.0f) {}
	Ray(const vec3& o, const vec3& d) : orig(o), dir(d) {}

	vec3 at(float t) const { return orig + t * dir; }
};

struct Sphere
{
	vec3 center;
	float radius;

	Sphere() : center(0.0f), radius(0.0f) {}
	Sphere(const vec3& c, float r) : center(c), radius(r) {}

	bool hit(const Ray& ray, float& t0) const {
		vec3 L = center - ray.orig;

		float a = dot(ray.dir, ray.dir);
		float b = -2.0f * dot(ray.dir, L);
		float c = dot(L, L) - radius * radius;

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0)
			return false;

		t0 = (-b - std::sqrt(discriminant)) / (2.0f * a);
		float t1 = (-b + std::sqrt(discriminant)) / (2.0f * a);
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		return true;
	}
};

vec3 castRay(const Ray& ray, const Sphere& sphere)
{
	float sphere_dist = std::numeric_limits<float>::max();
	// background color
	if (!sphere.hit(ray, sphere_dist))
		return vec3(0.2f, 0.3f, 0.8f);
	// sphere color
	return vec3(0.8f, 0.2f, 0.3f);
}

void render(const Sphere& sphere)
{
	const int width = 1280;
	const int height = 720;

	std::vector<vec3> framebuffer(width * height);

	for (uint32_t j = 0; j < height; ++j) {
		for (uint32_t i = 0; i < width; ++i) {
			float x = (2 * (i + 0.5f) / (float)width - 1.0f);
			float y = -(2 * (j + 0.5f) / (float)height - 1.0f);
			vec3 dir = vec3(x, y, -1).normalized();
			framebuffer[i + j * width] = castRay(Ray(vec3(0.0f), dir), sphere);
		}
	}

	std::ofstream ofs;
	ofs.open("out.ppm", std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (uint32_t i = 0; i < width * height; ++i) {
		for (uint32_t j = 0; j < 3; ++j) {
			ofs << (char)(255 * std::max(0.0f, std::min(1.0f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main()
{
	Sphere sphere(vec3(-3, 0, -16), 2);
	render(sphere);
	return 0;
}
