#pragma once


#include<memory>
#include<vector>

using std::shared_ptr;
using std::make_shared;

// define hittable_list, 使用共享指针来管理一系列的场景物体

class hittable_list : public hittable {
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

public:
	std::vector<shared_ptr<hittable>> objects;
};


// judge if hit with a list of objects
bool hittable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const 
{
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max; // 判断在场景中最近的碰撞点

	for (const auto & object : objects) // 引用调用迭代，牛的
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