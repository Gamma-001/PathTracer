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

	Sphere(vec3 position, float radius, Material* material) : MeshObject(material) {
		this->center = position;
		this->radius = radius;
	}
	
	/* 
		if a solution exists for : 
		dot((P - C), (P - C)) = pow(radius, 2); 
		where P : arbitary location on the ray, C : center of the sphere

		then the ray intersects the sphere
	*/
	virtual bool hit(const ray &r, float tMin, float tMax, HitRecord &rc) override;
};