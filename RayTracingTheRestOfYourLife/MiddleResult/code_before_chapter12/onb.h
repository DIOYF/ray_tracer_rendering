#pragma once
#include "vec3.h"


class onb {
public:
	onb() { build_from_w(vec3(0, 0, 1)); } // inital as z  (0,0,1)
	inline vec3 operator[](int i) const { return axis[i]; }
	vec3 u() const { return axis[0]; }
	vec3 v() const { return axis[1]; }
	vec3 w() const { return axis[2]; }

	// compute the vector expression of cordinate (a, b, c) in instanced onb 
	vec3 local(const vec3& a) const {
		return a.x() * u() + a.y() * v() + a.z() * w();
	}

	vec3 local(float a, float b, float c) const {
		return a * u() + b * v() + c * w();
	}

	

	void build_from_w(const vec3&);

public:
	vec3 axis[3];

};


void onb::build_from_w(const vec3& n) {
	axis[2] = unit_vector(n);
	vec3 a = (fabs(w().x()) > 0.9f) ? vec3(0, 1, 0) : vec3(1, 0, 0);
	axis[1] = unit_vector(cross(w(),a));
	axis[0] = cross(w(), v());
}