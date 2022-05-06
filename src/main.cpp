#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <numeric>
#include <limits>
#include <vector>
#include <cassert>

#include "vec3.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "util.hpp"
#include "material.hpp"

#include "bmp_24.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "./external/stb_image.h"	// I might use my own implementation later

std::vector <std::vector <vec3>> activeHDR;
float hdrExposure = 2.0f;
bool transparentBackground = true;

// controls the rotation of hdri along horizontal axis
float HDROffsetU = 0.6f;

// returns the color of the skybox [0-1]
vec3 SkyColor(const ray& r) {
	if (activeHDR.size() == 0) return vec3(1.0f, 1.0f, 1.0f);

	size_t HDR_width = activeHDR[0].size();
	size_t HDR_height = activeHDR.size();

	vec3 norm = r.direction.normal();
	int u = (HDR_width - 1) * (0.5f + atan2(norm.x, -norm.z) / (2.0f * PI));
	int v = (HDR_height - 1) * (0.5f + asin(norm.y) / PI);

	// add the offset
	u = (u + int(HDROffsetU * HDR_width)) % HDR_width;

	// assert(index < activeHDRHeight * activeHDRWidth * 3);
	return vec3(
		activeHDR[v][u].x * hdrExposure,
		activeHDR[v][u].y * hdrExposure,
		activeHDR[v][u].z * hdrExposure
	);
}

// this will be used for dielectrics
// returns the Shlick's approximation of fresnel equations at an incident angle
float Fresnel(vec3 a, vec3 n, float ior1, float ior2) {
	float R0 = pow((ior1 - ior2) / (ior1 + ior2), 2.0f);
	
	float cos_x = vec3::dot(a, n);
	// cos_x should always be positive
	if (cos_x < 0.0f) {
		cos_x = vec3::dot(a, -n);
	}

	float res = std::min(R0 + (1.0f - R0) * pow(1.0f - cos_x, 5.0f), 1.0f);

	return res; 
}

vec3 Trace(ray r, std::vector <Sphere> &spheres, vec3 rayColor = vec3(1.0f, 1.0f, 1.0f), float intensity = 1.0f, int depth = 0) {
	if (depth > 16 || int(intensity * 255) == 0) return vec3(0.0f, 0.0f, 0.0f);

	HitRecord hitRec;
	float best_t = std::numeric_limits <float> :: max();
	bool hit = false;

	for (auto &x: spheres) {
		HitRecord t_hrc;
		if (x.hit(r, 0.001f, std::numeric_limits <float> :: max(), t_hrc) && t_hrc.t < best_t) {
			hit = true;
			hitRec = t_hrc;
			best_t = t_hrc.t;
		}
	}

	if (hit) {
		float attenuation; 
		Material* mat = hitRec.object->material;

		ray newRay = mat->scatter(r, hitRec.position, hitRec.norm, attenuation);
		vec3 retColor = Trace(newRay, spheres, rayColor, intensity * attenuation, depth + 1);

		return vec3::multiply(intensity * mat->baseColor, retColor);
	}

	if (depth == 0 && transparentBackground) return vec3(0.01f, 0.01f, 0.01f);
	return SkyColor(r);
}

int main() {
	const float aspectRatio = 16.0f / 9.0f;

	const uint16_t imageWidth = 1920;
	const uint16_t imageHeight = imageWidth / aspectRatio;

	Diffuse diffRedMat(vec3(1.0f, 0.3f, 0.25f));
	Diffuse diffMat(vec3(1.0f, 1.0f, 1.0f));

	Glossy metalMat(vec3(1.0f, 1.0f, 1.0f), 0.05f);

	Camera viewCam(vec3(0.0f, 0.0f, 3.0f), 2.0f, aspectRatio * 2.0f, 3.0f);

	// list of objects placed int the scene
	std::vector <Sphere> spheres;

	spheres.push_back(Sphere(vec3(1.5f, 0.0f, -1.5f), 0.5f, static_cast <Material*> (&diffMat)));
	spheres.push_back(Sphere(vec3(-1.5f, 0.0f, -1.5f), 0.5f, static_cast <Material*> (&diffRedMat)));
	spheres.push_back(Sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, static_cast <Material*> (&metalMat)));

	spheres.push_back(Sphere(vec3(0.0f, -500.5f, -1.0f), 500.0f, static_cast <Material*> (&diffMat)));

	// samples per pixel
	int sampleCount = 64;

	// load the hdr and set it as active
	int hdriWidth = 0, hdriHeight = 0, hdriChannels = 0;
	stbi_set_flip_vertically_on_load(true);
	float *hdriData = stbi_loadf("assets/hdri/christmas_photo_studio_02_4k.hdr", &hdriWidth, &hdriHeight, &hdriChannels, 3);

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
		std::cout << "\rProgress: | " << std::string(20 - rem, '#') << std::string(rem, ' ') << " |" << std::flush;

		for (int j = 0; j < imageWidth; j++) {
			vec3 bgColor;

			for (int k = 0; k < sampleCount; k++) {
				float u = ((float)j + RandFloat()) / (imageWidth - 1);
				float v = ((float)i + RandFloat()) / (imageHeight - 1);

				ray r(viewCam.position, viewCam.llCorner + u * viewCam.horizontal + v * viewCam.vertical - viewCam.position);
				r.direction = r.direction.normal();
				
				bgColor += Trace(r, spheres).Clamp(0.0f, 1.0f);
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