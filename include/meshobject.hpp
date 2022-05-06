#pragma once

#include "ray.hpp"
#include "material.hpp"

static int meshCount = 0;

struct HitRecord;

class MeshObject {
private:
	// this will be useful later
	int id;
public:
	Material* material;
	
	MeshObject(Material *material) {
		id = meshCount;
		meshCount += 1;

		this->material = material;
	}

	virtual bool hit(const ray &r, float tMin, float tMax, HitRecord &rc) = 0;

	virtual ~MeshObject() {}

	const int GetId() const {
		return id;
	}
};

struct HitRecord {
	vec3 position;
	vec3 norm;
	float t;

	MeshObject* object;
};