#pragma once

#include <cmath>
#include <memory>
#include <limits>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;


// Constant Define

const float infinity = std::numeric_limits<float>::infinity(); // 从limit里获得无穷大的值
const float pi = 3.14159265358f;

// Transfer Funcition
inline float degrees_to_radians(float degrees)
{
	return degrees * pi / 180.0;
}

// Common Headers

// Random Function in rtweekend.h, using rand() function and RAND_MAX , now C++ has random function 

inline float random_float() 
{
	return rand() / (RAND_MAX + 1.0);
}


inline float random_float(float min, float max) 
{
	return min + random_float()* (max-min);
}

// dirty smooth function
inline float clamp(float x, float min, float max) 
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}


