#pragma once

#include "vec3.hpp"

class ray {
public:
	vec3 position;
	vec3 direction;

	inline ray() {}
	inline ray(const vec3 &pos , const vec3 &dir) {
		position = pos;
		direction = dir.normal();
	}

	// point on the ray for (lambda = t) in r = a + (lambda) b
	inline vec3 ray_at(float t) const {
		return position + t * direction.normal();
	}
};