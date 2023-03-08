#pragma once

#include "rtweekend.h"
#include "object.h"
#include "hittable_list.h"
#include <algorithm>
#include <iostream>

// 构建BVH树用于碰撞检测, 首先构建bvh结点，每个bvh结点包含一个可以进行碰撞检测的包围盒
// Note that the children pointers are to generic hittables. They can be other bvh_nodes, or spheres, or any other hittable.

class bvh_node : public hittable {
public:
	bvh_node();
	bvh_node(const hittable_list& list, float time0, float time1): bvh_node(list.objects, 0, list.objects.size(), time0, time1) {};

	bvh_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end, float time0, float time1);

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
	virtual bool bounding_box(float time0, float time1, aabb& output_box ) const override;

public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
};


bool bvh_node::bounding_box(float time0, float time1, aabb& output_box) const{
	output_box = box;
	return true;
} // 每个bvh的包围盒是它对应的结点的包围盒


bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	if (!box.hit(r, t_min, t_max)) return false;

	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec); // trick time

	return hit_left || hit_right;
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
	aabb box_a;
	aabb box_b;
	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) std::cerr << "No bounding box in bvh node constructor.\n";
	return box_a.min().e[axis] < box_b.min().e[axis];
}


bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
{
	return box_compare(a, b, 0);
}


bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
{
	return box_compare(a, b, 1);
}


bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
{
	return box_compare(a, b, 2);
}


// 构建完全二叉树bvh，随机选择一个坐标轴，排序全部的图元，一半作为左子树，一半作为右子树
// 对左右子树再分别进行相同操作，排序+二分
bvh_node::bvh_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end, float time0, float time1) {
	auto objects = src_objects;

	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare; // 用于大小比较的lamda函数

	size_t object_span = end - start;

	if (object_span == 1) {
		left = right = objects[start]; 
	}
	else if (object_span == 2)
	{
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;  // 排序结果二分，分别构建左子树，右子树, 注意end永远是最后一个元素索引再加一
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);
	}


	aabb box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left)
		|| !right->bounding_box(time0, time1, box_right)
		)
		std::cerr << "No bounding box in bvh_node constructor.\n";


	box = surrounding_box(box_left, box_right); // 对应node的包围盒
}


