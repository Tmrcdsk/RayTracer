#pragma once

#include "Hittable.h"

class Sphere : public Hittable
{
public:
	Sphere() : center(0.0f), radius(0.0f), material() {}
	Sphere(const vec3& c, float r, const Material& m) : center(c), radius(r), material(m) {}

	bool hit(const Ray& ray, float& tmin) const override {
		vec3 L = center - ray.orig;

		float a = dot(ray.dir, ray.dir);
		float b = -2.0f * dot(ray.dir, L);
		float c = dot(L, L) - radius * radius;

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0)
			return false;

		tmin = (-b - std::sqrt(discriminant)) / (2.0f * a);
		float tmax = (-b + std::sqrt(discriminant)) / (2.0f * a);
		if (tmin < 0) tmin = tmax;
		if (tmin < 0) return false;

		return true;
	}

	const vec3& GetCenter() const { return center; }
	const float& GetRadius() const { return radius; }
	const Material& GetMaterial() const { return material; }
private:
	vec3 center;
	float radius;
	Material material;
};