#pragma once

#include <vector>

class Camera {
private:
	float viewportHeight;
	float viewportWidth;
	float focalLength;

public:
	// camera origin
	vec3 position;
	
	// horizontal and vertical axis
	vec3 horizontal;
	vec3 vertical;

	// lower left corner of the camera, adjusted so 0,0,0 lies at the center of camera
	vec3 llCorner;

	inline Camera(vec3 pos, float height, float width, float focalLength) {
		position = pos;
		viewportHeight = height;
		viewportWidth = width;
		this->focalLength = focalLength;

		// initally the horizontal and vertical axis are towards +x and +y world space respectively
		// +z points towards the camera
		horizontal = vec3(viewportWidth, 0.0f, 0.0f);
		vertical = vec3(0.0f, viewportHeight, 0.0f);

		llCorner = position - horizontal / 2.0f - vertical / 2.0f - vec3(0.0f, 0.0f, focalLength);
	}
};