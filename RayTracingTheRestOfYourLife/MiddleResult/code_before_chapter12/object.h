#pragma once

#include "ray.h"
#include "vec3.h"
#include "aabb.h"
#include <cmath>

class material;


struct hit_record {
	point3 p; // interact point
	vec3 normal; // point normal
	shared_ptr<material> mat_ptr; // using shared pointer to record hit material info
	float t; // t value when inteact
	float u;
	float v;
	bool front_face; // judge which side ray interact with surface
};

// build bounding box for all hittable object
class hittable {
public:
	virtual bool hit(const ray&r, float t_min, float t_max, hit_record &rec) const=0;
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;
	virtual float pdf_value(const point3& o, const vec3& v) const {
		return 1.0f;
	}

	virtual vec3 random(const vec3& o) const {
		return vec3(0, 0, 0);
	} // 避免所有的子类都要实现，在基类里实现一个一般的pdf

};


class sphere : public hittable {
public:
	sphere(){}
	sphere(const point3& center, const float& radius, shared_ptr<material> m):center_value(center),radius_value(radius), mat_ptr(m) {}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;


	point3 center() { return center_value; }
	float radius() { return radius_value; }

private:
	static void get_sphere_uv(const point3& p, float & u, float & v) {
		auto theta = acos(-p.e[1]);
		auto phi = atan2(-p.e[2], p.e[0]) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}

private:
	point3 center_value;
	float radius_value;
	shared_ptr<material> mat_ptr;
};


// add a single sphere because hitting with a sphere is always easy
// Center and Radius to denote a sphere (X-C) .dot (X-C) =  R^2
// ray with the sphere : ||O+tD-C||^2 = R^2 
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
	if (discriminant < 0) return false; 
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
		get_sphere_uv(rec.normal, rec.u, rec.v);
		rec.mat_ptr = mat_ptr;
	}
	return true;
}


bool sphere::bounding_box(float time0, float time1, aabb& output_box) const {
	output_box = aabb(
		center_value - vec3(radius_value, radius_value, radius_value), 
		center_value + vec3(radius_value, radius_value, radius_value));
	return true;
}



// How to add a list of object which will be interact with our scene


class moving_sphere : public hittable {
public:
	moving_sphere(point3 cen0, point3 cen1, float _time0, float _time1, float r, shared_ptr<material> m)
	: center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m)
	{};

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
	virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override;
	point3 center(float time) const;

public:
	point3 center0, center1;
	float time0, time1;
	float radius;
	shared_ptr<material> mat_ptr;
};


// let you design your sphere
point3 moving_sphere::center(float time) const {
	return center0 + (time - time0)/(time1-time0) * (center1 - center0);
}


bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time()); // 移动物体的不同只在于光线发出时间，光线是光速，传播时间可忽略
	float a = r.direction().length_squared();
	float half_b = dot(oc, r.direction());
	float c = oc.length_squared() - radius * radius;
	float discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	float root = (-half_b - sqrtd) / a;
	if (root < t_min || root > t_max)
	{
		root = (half_b - sqrtd) / a;
		if (root < t_min || root > t_max)
		{
			return false;
		}
	}

	// 光线相交信息处理
	rec.t = root;
	rec.p = r.at(rec.t);
	vec3 outward_normal = (rec.p - center(r.time()))/ radius;
	rec.front_face = ( dot(r.direction(), outward_normal) < 0.0 );
	rec.normal = rec.front_face? outward_normal:  - outward_normal; // 注意面朝向和法线方向
	rec.mat_ptr = mat_ptr;
	return true;
}

bool moving_sphere::bounding_box(float _time0, float _time1, aabb& output_box) const {
	aabb box0( center(_time0) - vec3(radius, radius, radius), center(_time0) + vec3(radius, radius, radius));
	aabb box1( center(_time1) - vec3(radius, radius, radius), center(_time1) + vec3(radius, radius, radius));
	output_box = surrounding_box(box0, box1);
	return true;
}





class xy_rect : public hittable {
public:
	xy_rect(){}
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<material> mat) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}
	
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override {
		float t = (k - r.origin().z()) / r.direction().z();
		if (t < t_min || t> t_max) { return false; }

		float x = r.origin().x() + t * r.direction().x();
		float y = r.origin().y() + t * r.direction().y();

		if (x< x0 || x > x1 || y < y0 || y > y1) return false;

		rec.u = (x - x0) / (x1-  x0);
		rec.v = (y - y0) / (y1 - y0);
		rec.t = t;
		rec.mat_ptr = mp;
		rec.p = r.at(t);
		rec.normal = vec3(0,0,1);
		rec.front_face = dot(rec.normal, r.direction()) < 0.0;
		rec.normal = rec.front_face ? rec.normal : -rec.normal;

		return true;
	}
	virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override {
		output_box = aabb(point3(x0, y0, k - 0.001f), point3(x1, y1, k + 0.001f));
		return true;
	}


public:
	shared_ptr<material> mp;
	float x0, x1, y0, y1, k; // define the rectange position and size

};


class yz_rect : public hittable {
public:
	yz_rect() {}
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, shared_ptr<material> mat) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override {
		float t = (k - r.origin().x()) / r.direction().x();
		if (t < t_min || t> t_max) { return false; }

		float y = r.origin().y() + t * r.direction().y();
		float z = r.origin().z() + t * r.direction().z();

		if (y< y0 || y > y1 || z < z0 || z > z1) return false;

		rec.u = (y - y0) / (y1 - y0);
		rec.v = (z - z0) / (z1 - z0);
		rec.t = t;
		rec.mat_ptr = mp;
		rec.p = r.at(t);
		rec.normal = vec3(1, 0, 0);
		rec.front_face = dot(rec.normal, r.direction()) < 0.0;
		rec.normal = rec.front_face ? rec.normal : -rec.normal;

		return true;
	}
	virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override {
		output_box = aabb(point3(k - 0.001f,y0, z0), point3(k + 0.001f,y1, z1));
		return true;
	}


public:
	shared_ptr<material> mp;
	float y0, y1, z0, z1, k; // define the rectange position and size

};


class xz_rect : public hittable {
public:
	xz_rect() {}
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, shared_ptr<material> mat) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override {
		float t = (k - r.origin().y()) / r.direction().y();
		if (t < t_min || t> t_max) { return false; }

		float x = r.origin().x() + t * r.direction().x();
		float z = r.origin().z() + t * r.direction().z();

		if (x< x0 || x > x1 || z < z0 || z > z1) return false;

		rec.u = (x - x0) / (x1 - x0);
		rec.v = (z - z0) / (z1 - z0);
		rec.t = t;
		rec.mat_ptr = mp;
		rec.p = r.at(t);
		rec.normal = vec3(0, 1, 0);
		rec.front_face = dot(rec.normal, r.direction()) < 0.0;
		rec.normal = rec.front_face ? rec.normal : -rec.normal;

		return true;
	}
	virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override {
		output_box = aabb(point3(x0, k - 0.001f, z0), point3(x1, k + 0.001f, z1));
		return true;
	}


	// because of cornell box is xz_rect, so we achieve pdf light sampling in xz_rect
	virtual float pdf_value(const point3& origin, const vec3& v) const override {
		hit_record rec;
		if (!this->hit(ray(origin, v), 0.001f, infinity, rec))
			return 0;

		float area = (x1 - x0) * (z1 - z0);
		float distance_squared = rec.t * rec.t * v.length_squared(); // 通过光线相交位置估计
		float cosine = fabs(dot(v, rec.normal) / v.length());
		if (cosine < 0.000001) return 0;
		return distance_squared / ((cosine) * area); // D^2 / (cos(Alpha) * Area)
	}


	virtual vec3 random(const point3& origin) const override {
		point3 random_point = point3(random_float(x0, x1), k , random_float(z0, z1));
		return random_point - origin; // sample using uniform
	}


public:
	shared_ptr<material> mp;
	float x0, x1, z0, z1, k; // define the rectange position and size

};

//  move ray but not move object !!
/* how to rotate object, we build translate and rotation ray instance ,
An instance is a geometric primitive that has been moved or rotated somehow.
This is especially easy in ray tracing because we don’t move anything; 
instead we move the rays in the opposite direction.For example, consider a translation(often called a move).
We could take the pink box at the originand add 2 to all its x components, 
or (as we almost always do in ray tracing) leave the box where it is, 
but in its hit routine subtract 2 off the x - component of the ray origin.
*/

// 光线做一个逆向的平移就能判断相交，包围盒跟随变化
class translate : public hittable {
public:
	translate(shared_ptr<hittable> p, const vec3& displacement) : ptr(p), offset(displacement){}
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override {
		ray moved_ray(r.origin() - offset, r.direction(), r.time());
		if (!ptr->hit(moved_ray, t_min, t_max, rec)) return false;

		rec.p += offset;
		rec.front_face = dot(moved_ray.direction() , rec.normal) < 0.0;
		rec.normal = rec.front_face ? rec.normal : -rec.normal;
		return true;
	}
	virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override {
		if (!ptr->bounding_box(_time0, _time1, output_box)) return false;

		output_box = aabb(output_box.min() + offset , output_box.max() + offset ); 
	
	}

public:
	shared_ptr<hittable> ptr;
	vec3 offset;
};


class y_rotate : public hittable {
public:
	y_rotate(shared_ptr<hittable> p, float angle);

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
		output_box = bbox;
		return hasbox;
	}

public:
	shared_ptr<hittable> ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	aabb bbox;

};

//构建旋转过的包围盒bbox
y_rotate::y_rotate(shared_ptr<hittable> p, float angle) : ptr(p) 
{
	sin_theta = sinf(degrees_to_radians(angle));
	cos_theta = cosf(degrees_to_radians(angle));

	hasbox = ptr->bounding_box(0, 1, bbox);

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}

			}
		}
	}

	bbox = aabb(min, max);

}

// 光线进行旋转
bool y_rotate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const 
{
	vec3 origin = r.origin();
	vec3 direction = r.direction();

	// little ugly , we can use matrix transpose to denote rotation
	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	ray rotated_r(origin, direction, r.time());


	if (!ptr->hit(rotated_r, t_min, t_max, rec)) return false;

	point3 p = rec.p;
	vec3 normal = rec.normal;
	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];


	rec.p = p;
	rec.front_face = dot(normal, r.direction()) < 0.0;
	rec.normal = rec.front_face ? normal : -normal;

	return true;
}



// make light normal to y direction
class flip_face : public hittable { // 对于每根光线，只在打到光源下方时进行发光，原先的设计都是双面反射物体，所以不涉及这些
public:
	flip_face(shared_ptr<hittable> p) : ptr(p) {}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override{
		

		if (!ptr->hit(r, t_min, t_max, rec)) return false;

		rec.front_face = !rec.front_face;

		return true;
	}

	// wrap flip_face for hittable
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
		return ptr->bounding_box(time0, time1, output_box);
	}

public:
	shared_ptr<hittable> ptr;
};