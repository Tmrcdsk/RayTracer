#pragma once

#include "Vector.h"

struct Ray
{
	vec3 orig;
	vec3 dir;

	Ray() : orig(0.0f), dir(0.0f) {}
	Ray(const vec3& o, const vec3& d) : orig(o), dir(d) {}

	vec3 at(float t) const { return orig + t * dir; }
};