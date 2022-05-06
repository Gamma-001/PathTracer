#include "material.hpp"

ray Diffuse::scatter(const ray& r, const vec3& pos, const vec3& norm, float& attenuation) const {
	// choose a random vector lying on a sphere placed right above the intersection point
	vec3 diffuseVec = RandSpherePos();
	diffuseVec += pos + norm;
	diffuseVec = vec3(diffuseVec - pos).normal();

	// intensity would be maximum if the diffuse reflection is in the direction of the normal
	float cos_x = vec3::dot(norm, diffuseVec);
	attenuation = cos_x;

	return ray(pos, diffuseVec.normal());
}

ray Glossy::scatter(const ray& r, const vec3& pos, const vec3& norm, float& attenuation) const {
	vec3 diffuseVec = RandSpherePos();
	diffuseVec += pos + norm;
	diffuseVec = vec3(diffuseVec - pos).normal();

	vec3 specularVec = vec3::reflect(r.direction, norm).normal();
	vec3 combinedVec = vec3(roughness * diffuseVec + (1.0f - roughness) * specularVec).normal();

	float cos_x = vec3::dot(diffuseVec, norm);
	attenuation = 0.95 * ((1.0f - roughness) + roughness * cos_x);

	return ray(pos, combinedVec);
}