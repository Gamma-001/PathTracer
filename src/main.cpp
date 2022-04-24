#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <numeric>
#include <limits>
#include <vector>
#include <random>
#include <cassert>

#include "vec3.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "camera.hpp"

#include "bmp_24.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "./external/stb_image.h"	// I might use my own implementation later

constexpr float PI = 3.14159;

std::vector <std::vector <vec3>> activeHDR;
float hdrExposure = 1.0f;

// returns the color of the skybox [0-1]
vec3 SkyColor(const ray& r) {
	if (activeHDR.size() == 0) return vec3(1.0f, 1.0f, 1.0f);

	vec3 norm = 0.5f * (r.direction.normal() + vec3(1.0f, 1.0f, 1.0f));
	int x = norm.x * (activeHDR[0].size() - 1);
	int y = norm.y * (activeHDR.size() - 1);

	// assert(index < activeHDRHeight * activeHDRWidth * 3);
	return vec3(
		activeHDR[y][x].x * hdrExposure,
		activeHDR[y][x].y * hdrExposure,
		activeHDR[y][x].z * hdrExposure
	);
}

// returns a random floating point number between 0.0 and 1.0
inline float RandFloat() {
	static std::uniform_real_distribution <float> distribution(0.0f, 1.0f);
	static std::mt19937 gen;
	return distribution(gen);
}

// returns a random point a sphere
vec3 RandSpherePos(int radius = 1.0f) {
	float theta = RandFloat() * PI; 			// 0 < theta < 180
	float phi = RandFloat() * PI * 2.0f; 		// 0 < phi < 360

	return vec3(
		radius * cos(phi) * sin(theta),
		radius * sin(phi) * sin(theta),
		radius * cos(theta)
	);
}

// recusive ray tracing function, could be made iterative
vec3 LightBounce(ray r, std::vector <Sphere> &spheres, HitRecord hitRec, vec3 rayColor = vec3(1.0f, 1.0f, 1.0f), float roughness = 1.0f, float intensity = 1.0f, int depth = 0) {
	if (depth > 16 || intensity < 0.004f) return rayColor;

	// specifies the amount of the light the surface can reflect, right now its a constant
	float reflectance = 0.75f;

	// find a random point a unit sphere placed right above the collision point
	vec3 spherePos = RandSpherePos(1.0f);
	vec3 reflectedRay = -(r.position - hitRec.position) - 2.0f * vec3::dot(-(r.position - hitRec.position), hitRec.norm.normal()) * hitRec.norm.normal();
	spherePos += hitRec.position + hitRec.norm.normal();

	// caculate the new positions of both diffuse and specular ray
	ray diffuseRay(hitRec.position, vec3(spherePos - hitRec.position).normal());
	ray specularRay(hitRec.position, reflectedRay.normal());

	// simply adding the two rays with a factor (roughness) and (1 - roughness) can yield a believable result
	// the more the roughness, the more the ray inclines towards the random ray
	ray newRay(hitRec.position, vec3(specularRay.direction * (1.0f - roughness) + diffuseRay.direction * (roughness)).normal());

	// test if this new ray collides with any object (right now only spheres)
	float hitAgain = false;
	Sphere target;
	HitRecord bestRec;
	bestRec.t = std::numeric_limits<float>::max();

	for (auto &x: spheres) {
		HitRecord t_hrc;
		if (x.hit(newRay, 0.001f, std::numeric_limits <float>::max(), t_hrc) && t_hrc.t < bestRec.t) {
			hitAgain = true;
			bestRec = t_hrc;
			target = x;
		}
	}

	// in case a collision point was found, recusrively repeat the process
	if (hitAgain) {
		return intensity * LightBounce(
			newRay, 
			spheres, 
			bestRec,
			vec3::multiply(rayColor, reflectance * intensity * target.color),
			target.roughness,
			intensity * reflectance, 
			depth + 1
		);
	}

	// if no collisions could be found, return the skyColor multiplied by the ray intesnity
	// clamp the light color vlaue to prevent fireflies
	return vec3::multiply(intensity * SkyColor(newRay), rayColor).Clamp(0.0f, 1.0f);
}

int main() {
	const float aspectRatio = 16.0f / 9.0f;

	const uint16_t imageWidth = 1920;
	const uint16_t imageHeight = imageWidth / aspectRatio;

	Camera viewCam(vec3(0.0f, 0.0f, 2.0f), 2.0f, aspectRatio * 2.0f, 2.0f);

	// list of objects placed int the scene
	std::vector <Sphere> spheres;

	spheres.push_back(Sphere(vec3(1.5f, 0.0f, -1.5f), 0.5f, vec3(0.2f, 0.8f, 0.01f), 0.0f));
	spheres.push_back(Sphere(vec3(-1.5f, 0.0f, -1.5f), 0.5f, vec3(1.0f, 1.0f, 1.0f), 0.0f));
	spheres.push_back(Sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, vec3(0.7f, 0.2f, 0.1f), 1.0f));

	spheres.push_back(Sphere(vec3(0.0f, -500.5f, -1.0f), 500.0f, vec3(0.5f, 0.5f, 0.5f), 0.5f));

	// samples per pixel
	int sampleCount = 64;

	// load the hdr and set it as active
	int hdriWidth = 0, hdriHeight = 0, hdriChannels = 0;
	stbi_set_flip_vertically_on_load(true);
	float *hdriData = stbi_loadf("assets/hdri/pump_station_2k.hdr", &hdriWidth, &hdriHeight, &hdriChannels, 3);

	assert(hdriWidth != 0 && hdriHeight != 0);
	
	int hdriIndex = 0;
	for (int i = 0; i < hdriHeight; i++) {
		std::vector <vec3> hdrRow;
		for (int j = 0; j < hdriWidth; j++) {
			hdrRow.push_back(vec3(
				hdriData[hdriIndex],
				hdriData[hdriIndex + 1],
				hdriData[hdriIndex + 2]
			));

			hdriIndex += 3;
		}
		activeHDR.push_back(hdrRow);
	}

	// class for storing pixel data and writing to a bitmap
	Bitmap bmp(static_cast <uint32_t> (imageWidth), static_cast <uint32_t> (imageHeight));

	for (int i = imageHeight - 1; i >= 0; i--) {
		// progress indicator
		int rem = 20 * (float(i)/imageHeight);
		std::cerr << "\rProgress: | " << std::string(20 - rem, '#') << std::string(rem, ' ') << " |" << std::flush;

		for (int j = 0; j < imageWidth; j++) {
			vec3 bgColor;

			for (int k = 0; k < sampleCount; k++) {
				float u = ((float)j + RandFloat()) / (imageWidth - 1);
				float v = ((float)i + RandFloat()) / (imageHeight - 1);

				ray r(viewCam.position, viewCam.llCorner + u * viewCam.horizontal + v * viewCam.vertical - viewCam.position);

				// if the ray hit the sphere then color the pixel based on the sphere's normal at that position
				
				float bestT = std::numeric_limits <float> :: max();
				bool hitObject = false;

				HitRecord bestRec;
				bestRec.t = std::numeric_limits<float>::max();

				Sphere target;
				for (auto &x: spheres) {
					HitRecord t_hrc;
					if (x.hit(r, 0.001f, std::numeric_limits <float>::max(), t_hrc)) {
						if (t_hrc.t < bestRec.t) {
							hitObject = true;
							bestRec = t_hrc;
							target = x;
						}
					}
				}

				if (hitObject) {
					// shoot out N rays from the current hit location and average the color values
					bgColor += LightBounce(r, spheres, bestRec, target.color, target.roughness);
				}
				// if the ray didn't hit the sphere color the pixed withe background
				else {
					// bgColor += SkyColor(r);
					bgColor += vec3(0.01f, 0.01f, 0.01f);
				}
			}

			// gamma correction
			bgColor.x = pow(bgColor.x / (float)sampleCount, 0.5f);
			bgColor.y = pow(bgColor.y / (float)sampleCount, 0.5f);
			bgColor.z = pow(bgColor.z / (float)sampleCount, 0.5f);

			bgColor = bgColor.Clamp();

			// convert vector [0 - 1] to rgb [0 - 255]
			int8_t 	ir = static_cast <int8_t> (bgColor.x * 255),
					ig = static_cast <int8_t> (bgColor.y * 255),
					ib = static_cast <int8_t> (bgColor.z * 255);

			bmp[i][j] = BMPColor_rgb {ir, ig, ib};
		}
	}

	// write to the file after all operations are completed to prevent corruption
	std::fstream fsBmp("result.bmp", std::ios_base::out | std::ios_base::binary);
	bmp.writeHeader(fsBmp);
	bmp.writeColors(fsBmp);

	// close the image file after writing
	fsBmp.close();

	return 0;
}