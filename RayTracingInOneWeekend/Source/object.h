#pragma once

#include "ray.h"


using std::shared_ptr;
using std::make_shared;
using std::sqrt;


class material;


// ���ڹ��ߺ͸����������д��һ������ִ��
// update : һ���������������󽻵����ݽṹ, when interact with sphere, you can know what ray is
struct hit_record {
	point3 p; // interact point
	vec3 normal; // point normal
	shared_ptr<material> mat_ptr; // using shared pointer to record hit material info
	float t; // t value when inteact
	bool front_face; // judge which side ray interact with surface



};

class hittable {
public:
	virtual bool hit(const ray&r, float t_min, float t_max, hit_record &rec) const=0;
};



class sphere : public hittable {
public:
	sphere(){}
	sphere(const point3& center, const float& radius, shared_ptr<material> m):center_value(center),radius_value(radius), mat_ptr(m) {}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

	point3 center() { return center_value; }
	float radius() { return radius_value; }


private:
	point3 center_value;
	float radius_value;
	shared_ptr<material> mat_ptr;
};


// add a single sphere because hitting with a sphere is always easy
// Center and Radius to denote a sphere (X-C) .dot (X-C) =  R^2
// ray with the sphere : ||O+tD-C||^2 = R^2 

// �ཻ�Ľ����Ϣ�����hit_record& ��, ��Ҫ�жϹ���������ڻ��������
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	// judge if sphere hit with ray, 
	//and if hit compute the t where ray inter the sphere, this point could determine the surface normal
	// D.D * t^2 + 2 * D.(O-C) * t + ( O-C )^2 - R ^ 2 = 0
	vec3 OC = r.origin() - center_value;
	vec3 D = r.direction();
	float a = dot(D, D);
	float h = dot(D, OC);
	float c = dot(OC, OC) - radius_value * radius_value;
	float discriminant = h * h - a * c;
	if (discriminant < 0) return false; // �����о����ȵ����⣬�߽粻Ҫ���ǣ����������������������
	else
	{
		float t1 = (-h - sqrt(discriminant)) / a;
		if (t1 < t_min || t1 > t_max)
		{
			float t2 = (-h + sqrt(discriminant)) / a; // we assume that sphere is front of camera, so choose t1 is enough
			if (t2 < t_min || t2>t_max)
				return false;
			t1 = t2;
		}
		// t1 in (t_min,t_max)
		rec.p = r.at(t1);
		rec.normal = (rec.p - center_value)/radius_value;
		rec.t = t1;
		// if hit in back_face, normal direction reverse
		rec.front_face = (dot(D, rec.normal) < 0.0);
		rec.normal = rec.front_face? rec.normal:-rec.normal;
		rec.mat_ptr = mat_ptr;
	}
	return true;
}


// How to add a list of object which will be interact with our scene