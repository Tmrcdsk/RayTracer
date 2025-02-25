#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Vector.h"

#define PI 3.14159265358979323846

struct Material
{
	Material(const vec3& color) : diffuse_color(color) {}
	Material() : diffuse_color() {}
	vec3 diffuse_color;
};

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
	Material material;

	Sphere() : center(0.0f), radius(0.0f), material() {}
	Sphere(const vec3& c, float r, const Material& m) : center(c), radius(r), material(m) {}

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

bool scene_intersect(const Ray& ray, const std::vector<Sphere>& spheres, vec3& hitPoint, vec3& N, Material& material)
{
	float sphere_dist = std::numeric_limits<float>::max();
	for (auto& sphere : spheres) {
		float dist_i;
		if (sphere.hit(ray, dist_i) && dist_i < sphere_dist) {
			sphere_dist = dist_i;
			hitPoint = ray.at(dist_i);
			N = (hitPoint - sphere.center).normalized();
			material = sphere.material;
		}
	}
	return sphere_dist < 1000.0f;
}

vec3 castRay(const Ray& ray, const std::vector<Sphere>& spheres)
{
	vec3 point, N;
	Material material;
	// background color
	if (!scene_intersect(ray, spheres, point, N, material))
		return vec3(0.2f, 0.3f, 0.8f);
	// sphere color
	return material.diffuse_color;
}

void render(const std::vector<Sphere>& spheres)
{
	const int width = 1280;
	const int height = 720;
	float fov = (float)PI / 2; // 45 degree
	float aspect = (float)width / height;

	std::vector<vec3> framebuffer(width * height);

	for (uint32_t j = 0; j < height; ++j) {
		for (uint32_t i = 0; i < width; ++i) {
			float x = (2 * (i + 0.5f) / (float)width - 1.0f) * tan(fov / 2.0f) * aspect;
			float y = -(2 * (j + 0.5f) / (float)height - 1.0f) * tan(fov / 2.0f);
			vec3 dir = vec3(x, y, -1).normalized();
			framebuffer[i + j * width] = castRay(Ray(vec3(0.0f), dir), spheres);
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
	Material ivory(vec3(0.4f, 0.4f, 0.3f));
	Material red(vec3(0.8f, 0.2f, 0.3f));
	
	std::vector<Sphere> spheres;
	spheres.emplace_back(vec3(-3, 0, -16), 2, ivory);
	spheres.emplace_back(vec3(-1.0, -1.5, -12), 2, red);
	spheres.emplace_back(vec3(1.5, -0.5, -18), 3, red);
	spheres.emplace_back(vec3(7, 5, -18), 4, ivory);

	render(spheres);
	return 0;
}
