#pragma once

#include <iostream>
#include <cmath>

class vec3 {
public:
	float x, y, z;

	inline vec3() : x(0), y(0), z(0) {};
	
	inline vec3(float x, float y, float z) : x(x), y(y), z(z) {};

	// getters

	float length() const {
		return sqrt(length2());
	}

	float length2() const {
		return x * x + y * y + z * z;
	}

	// overloaded operators

	vec3 operator - () const;
	vec3 operator + (const vec3 &vec) const;
	vec3 operator - (const vec3 &vec) const;
	vec3 operator * (const float &s) const;
	vec3 operator / (const float &s) const;

	vec3& operator += (const vec3 &v);
	vec3& operator *= (const float &s);
	vec3& operator /= (const float &s);

	friend std::ostream& operator << (std::ostream &out , const vec3& v);	

	// utility functions

	static float dot(const vec3 &a, const vec3 &b);

	static vec3 cross(const vec3 &a, const vec3 &b);

	static vec3 multiply(const vec3 &a, const vec3 &b);

	vec3 normal() const;
	vec3 Clamp(float c_min = 0.0f, float c_max = 1.0f) const;
};

inline vec3 operator * (const float &s, const vec3 &v) {
	return v * s;
}