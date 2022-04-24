#pragma once

#include "meshobject.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#include <numeric>
#include <cmath>
#include <cassert>

class Sphere: public MeshObject {
public:
	vec3 center;
	float radius;

	Sphere() {}
	Sphere(vec3 pos, float r, vec3 color = vec3(1.0f, 1.0f, 1.0f), float rough = 0.5f) {
		center = pos;
		radius = r;
		roughness = rough;

		this->color = color;
	}
	
	/* 
		if a solution exists for : 
		dot((P - C), (P - C)) = pow(radius, 2); 
		where P : arbitary location on the ray, C : center of the sphere

		then the ray intersects the sphere
	*/
	virtual bool hit(const ray &r, float tMin, float tMax, HitRecord &rc) override;
};