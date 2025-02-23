#include <iostream>
#include <fstream>
#include <vector>

#include "Vector.h"

void render()
{
	const int width = 1280;
	const int height = 720;
	
	std::vector<vec3> framebuffer(width * height);

	for (uint32_t j = 0; j < height; ++j) {
		for (uint32_t i = 0; i < width; ++i) {
			framebuffer[i + j * width] = vec3(i / (float)width, j / (float)height, 0.0f);
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
	render();
	return 0;
}
