#pragma once

#include <iostream>
#include <limits>
#include <memory>

#include "Ray.h"
#include "Vector.h"

#define PI 3.14159265358979323846
constexpr float infinity = std::numeric_limits<float>::max();

inline float degrees_to_radians(float degrees) {
	return degrees * PI / 180.0f;
}
