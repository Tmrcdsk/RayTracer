#pragma once

#include <cmath>

struct vec4
{
	float x, y, z, w;

	vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	vec4(float x) : x(x), y(x), z(x), w(x) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	float& operator[](int index) { return index == 0 ? x : (index == 1 ? y : (index == 2 ? z : w)); }
	const float& operator[](int index) const { return index == 0 ? x : (index == 1 ? y : (index == 2 ? z : w)); }

	vec4 operator+(const vec4& other) const { return vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
	vec4 operator-(const vec4& other) const { return vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
	vec4 operator-() const { return vec4(-x, -y, -z, -w); }
	vec4 operator*(const vec4& other) const { return vec4(x * other.x, y * other.y, z * other.z, w * other.w); }
	vec4 operator*(float k) const { return vec4(k * x, k * y, k * z, k * w); }
	friend inline vec4 operator*(float k, const vec4& v);

	float norm() const { return sqrt(x * x + y * y + z * z + w * w); }
	float norm2() const { return x * x + y * y + z * z + w * w; }
	vec4 normalized() const
	{
		float length = norm();
		return vec4(x / length, y / length, z / length, w / length);
	}
};

inline vec4 operator*(float k, const vec4& v) { return vec4(k * v.x, k * v.y, k * v.z, k * v.w); }

struct vec3
{
	float x, y, z;

	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3(float x) : x(x), y(x), z(x) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	float& operator[](int index) { return index == 0 ? x : (index == 1 ? y : z); }
	const float& operator[](int index) const { return index == 0 ? x : (index == 1 ? y : z); }

	vec3 operator+(const vec3& other) const { return vec3(x + other.x, y + other.y, z + other.z); }
	vec3 operator-(const vec3& other) const { return vec3(x - other.x, y - other.y, z - other.z); }
	vec3 operator-() const { return vec3(-x, -y, -z); }
	vec3 operator*(const vec3& other) const { return vec3(x * other.x, y * other.y, z * other.z); }
	vec3 operator*(float k) const { return vec3(k * x, k * y, k * z); } // ÓÒ³Ë
	friend inline vec3 operator*(float k, const vec3& v); // ×ó³Ë

	float norm() const { return sqrt(x * x + y * y + z * z); }
	float norm2() const { return x * x + y * y + z * z; }
	vec3 normalized() const
	{
		float length = norm();
		return vec3(x / length, y / length, z / length);
	}
};

inline vec3 operator*(float k, const vec3& v) { return vec3(k * v.x, k * v.y, k * v.z); }

inline float dot(const vec3& a, const vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline vec3 cross(const vec3& a, const vec3& b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

struct vec2
{
	float x, y;

	vec2() : x(0.0f), y(0.0f) {}
	vec2(float x) : x(x), y(x) {}
	vec2(float x, float y) : x(x), y(y) {}

	float& operator[](int index) { return index == 0 ? x : y; }
	const float& operator[](int index) const { return index == 0 ? x : y; }

	vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y); }
	vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y); }
	vec2 operator-() const { return vec2(-x, -y); }
	vec2 operator*(const vec2& other) const { return vec2(x * other.x, y * other.y); }
	vec2 operator*(float k) const { return vec2(k * x, k * y); } // ÓÒ³Ë
	friend inline vec2 operator*(float k, const vec2& v); // ×ó³Ë

	float norm() const { return sqrt(x * x + y * y); }
	float norm2() const { return x * x + y * y; }
	vec2 normalized() const
	{
		float length = norm();
		return vec2(x / length, y / length);
	}
};

inline vec2 operator*(float k, const vec2& v) { return vec2(k * v.x, k * v.y); }
