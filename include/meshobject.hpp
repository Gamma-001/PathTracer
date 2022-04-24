#pragma once

#include "ray.hpp"

struct HitRecord {
	vec3 position;
	vec3 norm;
	float t;
};

class MeshObject {
public:
	vec3 color;
	float roughness;

	virtual bool hit(const ray &r, float tMin, float tMax, HitRecord &rc) = 0;
};