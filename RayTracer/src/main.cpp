#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Vector.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PI 3.14159265358979323846

int envmap_width, envmap_height;
std::vector<vec3> envmap;

struct Light
{
	Light(const vec3& p, float i) : position(p), intensity(i) {}
	vec3 position;
	float intensity;
};

struct Material
{
	Material(float r, const vec4& a, const vec3& color, float spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
	Material() : refractive_index(1.0f), albedo(1, 0, 0, 0), diffuse_color(), specular_exponent() {}
	float refractive_index;
	vec4 albedo;
	vec3 diffuse_color;
	float specular_exponent;
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

// 特别说明：这里的reflect函数的入射方向是由点指向光源的，而refract函数的入射方向则是由光源指向点
vec3 reflect(const vec3& L, const vec3& N)
{
	return 2 * dot(L, N) * N - L;
}

vec3 refract(const vec3& L, const vec3& N, float refractive_index)
{
	float cosi = -std::max(-1.0f, std::min(1.0f, dot(L, N)));
	float etai = 1, etat = refractive_index;
	vec3 n = N;
	if (cosi < 0) {
		cosi = -cosi;
		std::swap(etai, etat);
		n = -N;
	}
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? vec3(0.0f) : eta * L + (eta * cosi - sqrtf(k)) * n;
}

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

	float checkerboard_dist = std::numeric_limits<float>::max();
	if (fabs(ray.dir.y) > 1e-3) {
		float d = -(ray.orig.y + 4) / ray.dir.y; // the checkerboard plane has equation y = -4
		vec3 pt = ray.orig + ray.dir * d;
		if (d > 0 && fabs(pt.x) < 10 && pt.z < -10 && pt.z > -30 && d < sphere_dist) {
			checkerboard_dist = d;
			hitPoint = pt;
			N = vec3(0.0f, 1.0f, 0.0f);
			material.diffuse_color = (int(0.5f * hitPoint.x + 1000) + int(0.5f * hitPoint.z)) & 1 ? vec3(1.0f) : vec3(1.0f, 0.7f, 0.3f); // 设置为棋盘图案
			material.diffuse_color = material.diffuse_color * 0.3f;
		}
	}
	return std::min(sphere_dist, checkerboard_dist) < 1000.0f;
}

vec3 castRay(const Ray& ray, const std::vector<Sphere>& spheres, const std::vector<Light>& lights, size_t depth = 0)
{
	vec3 point, N;
	Material material;
	// background color
	if (depth > 4 || !scene_intersect(ray, spheres, point, N, material)) {
		// background color
		float phi = atan2(ray.dir.z, ray.dir.x); // [-π, π]
		float theta = acos(ray.dir.y); // [0, π]
		// ensure x and y in range
		int x = std::max(0, std::min(envmap_width - 1, int((phi + PI) / (2 * PI) * envmap_width)));
		int y = std::max(0, std::min(envmap_height - 1, int(theta / PI * envmap_height)));
		return envmap[x + y * envmap_width];
	}
	/*
	- atan2：返回值 [-π, π]
		- 几何意义：atan2 返回的是 从 X 轴正方向逆时针旋转到向量 (x, z) 的角度，正值为逆时针方向（0 → π），负值为顺时针方向（-0 → -π）。
		- 符号一致性：当 x < 0 时（即向量指向 X 轴负方向），atan2 会自动将角度偏移 ±π，确保结果始终在 [-π, π] 内。
		- 例如：
		- 光线方向沿 X 正轴（x=1, z=0）→ φ = 0
		- 光线方向沿 X 负轴（x=-1, z=0）→ φ = π
		- 光线方向沿 Z 正轴（x=0, z=1）→ φ = π/2
		- 光线方向沿 Z 负轴（x=0, z=-1）→ φ = -π/2
	- acos：返回值 [0, π]
		- acos(1) = 0
		- acos(-1) = π
	 */

	vec3 reflect_dir = reflect(-ray.dir, N).normalized();
	vec3 reflect_orig = dot(reflect_dir, N) < 0 ? point - N * 0.001f : point + N * 0.001f; // 修改了一个小错误
	vec3 reflect_color = castRay(Ray(reflect_orig, reflect_dir), spheres, lights, depth + 1);
	
	vec3 refract_dir = refract(ray.dir, N, material.refractive_index).normalized();
	vec3 refract_orig = dot(refract_dir, N) < 0 ? point - N * 0.001f : point + N * 0.001f;
	vec3 refract_color = castRay(Ray(refract_orig, refract_dir), spheres, lights, depth + 1);

	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for (uint32_t i = 0; i < lights.size(); ++i) {
		vec3 light_dir = (lights[i].position - point).normalized();
		float light_distance = (lights[i].position - point).norm();

		vec3 shadow_orig = dot(light_dir, N) < 0 ? point - N * 0.001f : point + N * 0.001f; // 防止阴影自相交
		vec3 shadow_pt, shadow_N;
		Material tmp;
		if (scene_intersect(Ray(shadow_orig, light_dir), spheres, shadow_pt, shadow_N, tmp) && (shadow_pt - shadow_orig).norm() < light_distance)
			continue;

		diffuse_light_intensity += lights[i].intensity * std::max(0.0f, dot(light_dir, N));
		specular_light_intensity += lights[i].intensity * powf(std::max(0.0f, dot(reflect(light_dir, N), -ray.dir)), material.specular_exponent);
	}
	// sphere color
	return diffuse_light_intensity * material.diffuse_color * material.albedo[0]
		+ specular_light_intensity * vec3(1.0f) * material.albedo[1]
		+ reflect_color * material.albedo[2]
		+ refract_color * material.albedo[3];
}

void render(const std::vector<Sphere>& spheres, const std::vector<Light>& lights)
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
			framebuffer[i + j * width] = castRay(Ray(vec3(0.0f), dir), spheres, lights);
		}
	}

	std::vector<unsigned char> pixmap(width * height * 3);

	for (uint32_t i = 0; i < width * height; ++i) {
		vec3& c = framebuffer[i];
		float max = std::max(c[0], std::max(c[1], c[2]));
		if (max > 1) c = c * (1.0f / max);
		for (uint32_t j = 0; j < 3; ++j) {
			pixmap[i * 3 + j] = (unsigned char)(255 * std::max(0.0f, std::min(1.0f, framebuffer[i][j])));
		}
	}
	stbi_write_jpg("out.jpg", width, height, 3, pixmap.data(), 100);
}

int main()
{
	int channel = -1;
	unsigned char* pixmap = stbi_load("./envmap.jpg", &envmap_width, &envmap_height, &channel, 0);
	if (!pixmap || channel != 3) {
		std::cerr << "Error: can not load the environment map!" << std::endl;
		return -1;
	}
	envmap.reserve(envmap_width * envmap_height);
	for (int j = envmap_height - 1; j >= 0; --j) {
		for (int i = 0; i < envmap_width; ++i) {
			envmap[i + j * envmap_width] = vec3(pixmap[(i + j * envmap_width) * 3 + 0], pixmap[(i + j * envmap_width) * 3 + 1], pixmap[(i + j * envmap_width) * 3 + 2]) * (1.0f / 255.0f);
		}
	}
	stbi_image_free(pixmap);

	Material ivory(1.0f, vec4(0.6f, 0.3f, 0.1f, 0.0f), vec3(0.4f, 0.4f, 0.3f), 50.0f);
	Material red(1.0f, vec4(0.9f, 0.1f, 0.0f, 0.0f), vec3(0.3f, 0.1f, 0.1f), 10.0f);
	Material mirror(1.0f, vec4(0.0f, 10.0f, 0.8f, 0.0f), vec3(1.0f), 1425.0f);
	Material glass(1.5f, vec4(0.0f, 0.5f, 0.1f, 0.8f), vec3(0.6f, 0.7f, 0.8f), 125.0f);

	std::vector<Sphere> spheres;
	spheres.emplace_back(vec3(-3, 0, -16), 2, ivory);
	spheres.emplace_back(vec3(-1.0, -1.5, -12), 2, glass);
	spheres.emplace_back(vec3(1.5, -0.5, -18), 3, red);
	spheres.emplace_back(vec3(7, 5, -18), 4, mirror);

	std::vector<Light> lights;
	lights.emplace_back(vec3(-20, 20, 20), 1.5f);
	lights.emplace_back(vec3(30, 50, -25), 1.8f);
	lights.emplace_back(vec3(30, 20, 30), 1.7f);

	render(spheres, lights);
	return 0;
}
