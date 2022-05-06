#pragma once

#include <vector>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"
#include "util.hpp"

// Interface for all physical materials
class Material {
public:
	vec3 baseColor;

	Material(vec3 baseColor) {
		this->baseColor = baseColor;
	}

	/**
	 * @brief returns a ray scatterd from a point based on the material properties
	 * @param ray:  incident ray
	 * @param vec3: point of intersection
	 * @param vec3: normal of the object on the position
	 * @param vec3: color of the ray, it would be modified to the new value
	 * @param float: intensity multiplier for the resultant ray
	 */
	virtual ray scatter(const ray& r, const vec3& pos, const vec3& norm, float &attenuation) const = 0;
};

class Diffuse: public Material {
public:
	Diffuse(vec3 baseColor) : Material(baseColor) { }

	ray scatter(const ray& r, const vec3& pos, const vec3& norm, float& attenuation) const;
};

class Glossy: public Material {
public:
	float roughness;

	Glossy(vec3 baseColor, float roughness = 0.0f) : Material(baseColor) {
		this->roughness = roughness;
	}

	ray scatter(const ray& r, const vec3& pos, const vec3& norm, float& attenuation) const;
};