#pragma once

#include <random>
#include <numeric>
#include "vec3.hpp"

constexpr float PI = 3.14159;

// returns a random floating point number in the range
float RandFloat(float start = 0.0f, float end = 1.0f);

// returns a random integer in the range
int RandInt(int start, int end);

// returns a random point on a sphere
vec3 RandSpherePos(float radius = 1.0f);

// clamp a floating point number
float Clampf(float val, float _min, float _max);