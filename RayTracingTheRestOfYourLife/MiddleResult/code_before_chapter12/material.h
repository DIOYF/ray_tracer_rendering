#pragma once

// material determine when ray interact with sphere , what happen next

#include "ray.h"
#include "object.h"
#include "texture.h"
#include "onb.h"
struct hit_record;

// base class of material
class material {
public:
	// scatter 
	/*
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const = 0;
	*/
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const = 0;

	virtual float scattering_pdf(
		const ray& r_in, const hit_record& rec,ray& scattered
	) const {
		return 0;
	};

	// normal no emitted light
	virtual color emitted(const ray& r_in, const hit_record& rec, float u, float v, const point3& p) const {
		return color(0, 0, 0);
	}
};


// modelling diffuse material
// update constant color with texture 

class lambertian :public material {
public:
	lambertian(const color & a) : albedo(make_shared<solid_color>(a)){}
	lambertian(shared_ptr<texture> a) : albedo(a){}
	/*virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		auto scatter_direction = rec.normal + random_unit_vector();
		// if rec.normal = - random unit vetor, compute error, so judge it
		if (scatter_direction.near_zero()) scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, r_in.time()); // note now ray has time property
		attenuation = albedo->value(rec.u, rec.v, rec.p); // 通过纹理采样获得颜色
		return true; 
	}
	*/

	
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const override {
		// no need ONB
		auto scatter_direction = rec.normal + random_unit_vector();
		// if rec.normal = - random unit vetor, compute error, so judge it
		if (scatter_direction.near_zero()) scatter_direction = rec.normal;

		scatter_direction = normalized(scatter_direction);
		scattered = ray(rec.p, scatter_direction, r_in.time()); // note now ray has time property

		attenuation = albedo->value(rec.u, rec.v, rec.p); // 通过纹理采样获得颜色
		
		pdf = dot(rec.normal, scattered.direction()) / ( scattered.direction().length() * pi); // 计算逆向概率值

		return true;
	}
	

	/*
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const override {
		auto scatter_direction = random_in_hemisphere(rec.normal);
		// if rec.normal = - random unit vetor, compute error, so judge it
		if (scatter_direction.near_zero()) scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, r_in.time()); // note now ray has time property，A项，不断迭代

		attenuation = albedo->value(rec.u, rec.v, rec.p); // Color项

		pdf = 0.5f / pi; // 计算逆向概率值

		return true;
	}
	*/

	// 兰伯特表面反射光线的采样S(direction)的值，刚好等于cos(theta) / pi, 半球面
	virtual float scattering_pdf(
		const ray& r_in, const hit_record& rec, ray& scattered
	) const {
		//scattering_pdf 的概率密度分部
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 : cosine/pi;   // scatter 的概率分部 cos_theta / pi， s(direction)项 
	};


public:
	shared_ptr<texture> albedo;

};


// return the result of mirror reflection
vec3 reflect(const vec3& ray_in, const vec3& normal)
{
	return ray_in - 2 * dot(ray_in, normal) * normal;
}


float reflectance(const float& cos_theta, const float& ref_ratio)
{
	// Use Schlick's approximation for reflectance. 
	float r0 = (1 - ref_ratio) / (1 + ref_ratio);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cos_theta), 5);

}

// return the result of refract ray
vec3 refract(const vec3& ray_in, const vec3& normal,float refraction_ratio)
{
	float cos_theta = fmin(dot(-ray_in, normal), 1.0); 
	float sin_theta = sqrtf(1 - cos_theta * cos_theta);



	bool cannot_refract = refraction_ratio * sin_theta > 1.0;
	// reflectance is a thirk to represent some lights is reflect, and some is refract
	if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float()) return reflect(ray_in, normal);
	vec3 r_out_prep = refraction_ratio * ( ray_in + cos_theta * normal );
	vec3 r_out_parallel = -sqrt(1 - r_out_prep.length_squared()) * normal;

	return r_out_prep + r_out_parallel;
}


// modelling metal material
class metal :public material {
public:
	metal(const color& a, float f):albedo(a),fuzz(f<1?f:1) {}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const override {
		vec3 reflected = reflect(normalized(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time()); // fuzz & random_in_unit_sphere
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0.0); 
	}
	// 在球面上进行采样



public:
	color albedo;
	float fuzz;
};


class dielectric :public material {
public:
	dielectric(float index_of_refraction): ir(index_of_refraction) {}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const override {
		attenuation = color(1.0f, 1.0f, 1.0f);
		float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		vec3 unit_direction = unit_vector(r_in.direction());
		vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);
		scattered = ray(rec.p, refracted, r_in.time());
		return true;
	}
public:
	float ir;
};


// Emissive Material , using Emissive Material to create raytracing
// physically based light: has position and size
class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const override {
		return false;
	}

	virtual color emitted(const ray& r_in, const hit_record& rec, float u, float v, const point3& p) const override {
		// only light down
		if (!rec.front_face)
			return color(0, 0, 0);
		return emit->value(u, v, p);
	}


public:
	shared_ptr<texture> emit;


};


// 各向同性散射材质
class isotropic : public material {

public:
	isotropic(color c) : albedo(make_shared<solid_color> (c) ) {}
	isotropic(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, float& pdf
	) const override {
		scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}


public:
	shared_ptr<texture> albedo;

};






