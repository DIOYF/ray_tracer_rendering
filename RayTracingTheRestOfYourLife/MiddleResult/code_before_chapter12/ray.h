#pragma once


// ray is discribed by a original point and its direction P = O + t * D


class ray{
public:
	ray(){}
	ray(const point3& ori, const vec3& dir, float time = 0.0f): origin_value(ori), direction_value(dir), tm(time) {}
	// t is time to judge phonto position
	point3 origin() const { return origin_value; }
	vec3 direction() const { return direction_value; }
	// add time for motion blur
	float time() const { return tm; }

	point3 at(float t) const  {
		return origin_value + direction_value * t;
	}

private:
	point3 origin_value;
	vec3 direction_value;
	float tm;
};
