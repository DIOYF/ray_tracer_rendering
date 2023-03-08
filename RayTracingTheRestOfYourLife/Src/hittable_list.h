#pragma once

#include "object.h"
#include<memory>
#include<vector>

using std::shared_ptr;
using std::make_shared;

/* The ray - object intersection is the main time - bottleneck in a ray tracer, Bounding Volume Hierarchies,
time is linear with the num of objects
排序系列的排序方式：划分空间或者划分对象， 后者更容易编写代码
boundding box need a little padding to escape grazing cases

*/

class hittable_list : public hittable {
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
	virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override;

	float pdf_value(const point3& o, const vec3& v) const {
		float weight = 1.0f / objects.size();
		float sum = 0.0f;

		for (const auto& object : objects)
			sum += weight * object->pdf_value(o, v);

		return sum;
	}

	vec3 random(const vec3& o) const {
		int int_size = static_cast<int>(objects.size());
		return objects[random_int(0, int_size - 1)]->random(o); // 随机选择一种object的概率，朝向object发射光线
	}



public:
	std::vector<shared_ptr<hittable>> objects;
};


// judge if hit with a list of objects
bool hittable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const 
{
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max; 

	for (const auto & object : objects) //
	{
		if (object->hit(r, t_min, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}


bool hittable_list::bounding_box(float _time0, float _time1, aabb& output_box) const
{
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;

	for (auto object: objects)
	{
		if (! object->bounding_box(_time0, _time1, temp_box) ) return false;
		output_box = first_box?temp_box : surrounding_box(output_box, temp_box);
		first_box = false;
	}

	return true;
}