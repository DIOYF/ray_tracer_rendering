#pragma once

#include "vec3.h"

// ray is discribed by a original point and its direction P = O + t * D
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

class ray{
public:
	ray(){}
	ray(const point3& ori, const vec3& dir): origin_value(ori), direction_value(dir) {} // ���캯���������������ܶ���Ϊ�����������������治��ʹ�������޶���
	// t is time to judge phonto position
	point3 origin() const { return origin_value; }
	vec3 direction() const { return direction_value; }

	point3 at(float t) const  {
		return origin_value + direction_value * t;
	}

private:
	point3 origin_value;
	vec3 direction_value;

};
