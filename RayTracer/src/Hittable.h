#pragma once

#include "global.h"

struct Material
{
	Material(float r, const vec4& a, const vec3& color, float spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
	Material() : refractive_index(1.0f), albedo(1, 0, 0, 0), diffuse_color(), specular_exponent() {}
	float refractive_index;
	vec4 albedo;
	vec3 diffuse_color;
	float specular_exponent;
};

struct HitPayload
{
	vec3 p;
	vec3 normal;
	float t;
	Material material;
	bool front_face;

	void set_face_normal(const Ray& ray, const vec3& outward_normal)
	{
		// Sets the hit record normal vector.
		// NOTE: the parameter `outward_normal` is assumed to have unit length.
		front_face = dot(ray.dir, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

class Hittable
{
public:
	virtual ~Hittable() = default;

	virtual bool hit(const Ray& ray, float& tmin) const = 0;
};
