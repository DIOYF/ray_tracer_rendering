#pragma once


#include "rtweekend.h"

#include "hittable_list.h"
#include "material.h"
#include "texture.h"

// 相变函数,体积雾处理
class constant_medium : public hittable {
public:
	constant_medium(shared_ptr<hittable> _boundary,float density, color a):
	boundary(_boundary), neg_inv_density(-1/density),phase_function(make_shared<isotropic>(a))
	{}

	constant_medium(shared_ptr<hittable> _boundary,float density ,shared_ptr<texture> a):
	boundary(_boundary), neg_inv_density(-1/density), phase_function(make_shared<isotropic>(a))
	{}


	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
		return boundary->bounding_box(time0, time1, output_box);
	}

public:
	shared_ptr<hittable> boundary;
	shared_ptr<material> phase_function;
	float neg_inv_density;  // 雾的密度

};



// this function is import to understand fog scatter, assume emit in and out the boundary
//  this particular implementation will work for boundaries like boxes or spheres, but will not work with toruses or shapes that contain voids. 
bool constant_medium::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	hit_record rec1, rec2;

	if (!boundary->hit(r, -infinity, infinity, rec1)) return false;
	if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2)) return false;


	if (rec1.t < t_min) rec1.t = t_min; // 进入volume处理
	if (rec2.t < t_max) rec2.t = t_max; // 射出volume处理

	if (rec1.t >= rec2.t) return false;

	if (rec1.t < 0) rec1.t = 0;

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length; // 处在体积雾中的距离
	const auto hit_distance = neg_inv_density * logf(random_float()); // 

	if (hit_distance > distance_inside_boundary) return false;

	rec.t = rec1.t + hit_distance / ray_length;
	rec.p = r.at(rec.t);

	rec.normal = vec3(1, 0, 0); // artitary
	rec.front_face = true;  // also arbitary 
	rec.mat_ptr = phase_function;

	return true;
}


