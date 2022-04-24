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

	root = std::min(r_a, r_b);
	// return the the point closer to the ray

	if (root < tMin || root > tMax) return false;

	// if (vec3::dot(rc.norm, r.direction) < 0) return false;

	rc.t = root;
	rc.position = r.ray_at(root);
	rc.norm = (rc.position - center);
	rc.norm = rc.norm.normal();

	return true;
}