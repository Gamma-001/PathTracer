#include "sphere.hpp"

bool Sphere::hit(const ray &r, float tMin, float tMax, HitRecord &rc) {
	vec3 oc = r.position - center;

	float a = r.direction.normal().length2();
	float b = 2.0f * vec3::dot(r.direction.normal(), oc);
	float c = oc.length2() - radius * radius;

	float d = b * b - 4 * a * c;

	if (d < 0) return false;

	float sq_d = sqrt(d);
	float root = 0.0f;

	// find the nearest root that lies in the acceptable range
	float r_a = (-b - sq_d) / (2.0f * a);
	float r_b = (-b + sq_d) / (2.0f * a);

	if (r_a < r_b && r_a > tMin && r_a < tMax) {
		root = r_a;
	}
	else if(r_b > tMin && r_b < tMax) {
		root = r_b;
	}
	else return false;
	
	// if (vec3::dot(rc.norm, r.direction) < 0) return false;

	rc.t = root;
	rc.position = r.ray_at(root);
	rc.norm = (rc.position - center);
	rc.norm = rc.norm.normal();
	rc.object = static_cast <MeshObject*> (this);

	return true;
}