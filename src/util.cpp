#include "util.hpp"

float RandFloat(float start, float end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution <float> distribution(start, end);
	return distribution(gen);
}

int RandInt(int start, int end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution <> distribution(start, end);

	return distribution(gen);
}

vec3 RandSpherePos(float radius) {
	float theta = RandFloat() * PI; 			// 0 < theta < 180
	float phi = RandFloat() * PI * 2.0f; 		// 0 < phi < 360

	return vec3(
		radius * cos(phi) * sin(theta),
		radius * sin(phi) * sin(theta),
		radius * cos(theta)
	);
}

float Clampf(float val, float _min, float _max) {
	return std::min(std::max(val, _min), _max);
}