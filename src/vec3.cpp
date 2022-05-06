#include "vec3.hpp"

vec3 vec3::operator - () const {
	return vec3 ( -x, -y, -z );
}

vec3 vec3::operator + (const vec3 &vec) const {
	return vec3(x + vec.x, y + vec.y, z + vec.z);
}

vec3 vec3::operator - (const vec3 &vec) const {
	return *this + (-vec);
}

vec3 vec3::operator * (const float &s) const {
	return vec3(x * s, y * s, z * s);
}

vec3 vec3::operator / (const float &s) const {
	return *this * (1.0f / s);
}

vec3& vec3::operator += (const vec3 &v) {
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

vec3& vec3::operator *= (const float &s) {
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

vec3& vec3::operator /= (const float &s) {
	*this *= 1/s;
	return *this;
}

float vec3::dot(const vec3 &a, const vec3 &b) {
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

vec3 vec3::cross(const vec3 &a, const vec3 &b) {
	return vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x 
	);
}

vec3 vec3::multiply(const vec3 &a, const vec3 &b) {
	return vec3(
		a.x * b.x,
		a.y * b.y,
		a.z * b.z
	);
}

vec3 vec3::reflect(const vec3 &a, const vec3 &n) {
	return a - 2.0f * dot(a, n) * n;
}

vec3 vec3::refract(vec3 a, vec3 n, float ior, bool &tir) {
	a = a.normal();
	n = n.normal();

	float cos_x = dot(a, -n);
	// cos_x should always be positive
	if (cos_x < 0.0f) {
		cos_x *= -1.0f;
		n *= -1.0f;
	}

	// total internal reflection, so return the reflected ray
	if (1.0f - ior * ior * (1.0f - cos_x * cos_x) < 0.0f) {
		tir = true;
		return a - 2.0f * dot(a, n) * n;
	}

	vec3 r_a = ior * a + ior * cos_x * n;
	vec3 r_b = -sqrt(1.0f - ior * ior * (1.0f - cos_x * cos_x)) * n;

	tir = false;
	return r_a + r_b;
}

std::ostream& operator << (std::ostream &out , const vec3& v) {
	out << '[' << round(v.x * 10.0f) / 10.0f << ' ' << round(v.y * 10.0f) / 10.0f << ' ' << round(v.z * 10.0f) / 10.0f << ']';
	return out;
}

vec3 vec3::normal() const {
	float len = length();
	if (len == 0.0f) return vec3(0.0f, 0.0f, 0.0f);

	return vec3 (
		x / len,
		y / len,
		z / len
	);
}

vec3 vec3::Clamp(float c_min, float c_max) const {
	return vec3(
		((x < c_min) ? c_min : ((x > c_max) ? c_max : x)),
		((y < c_min) ? c_min : ((y > c_max) ? c_max : y)),
		((z < c_min) ? c_min : ((z > c_max) ? c_max : z))
	);
}