#pragma once

struct vec3
{
	float x, y, z;

	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3(float x) : x(x), y(x), z(x) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	float& operator[](int index) { return index == 0 ? x : (index == 1 ? y : z); }
	const float& operator[](int index) const { return index == 0 ? x : (index == 1 ? y : z); }
};
