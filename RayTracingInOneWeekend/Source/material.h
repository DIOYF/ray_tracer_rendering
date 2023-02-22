#pragma once

// 材质属性目前决定反射光的方向，反射光的颜色两种属性，对应的信息记录到scattered

// 非自发光漫反射材质的物体显示的是它们周围的颜色，但是它们自身的颜色也会对最终颜色进行调制（module）
// 漫反射材质的物体反射光的方向是随机的，它们的反射光会显示完全随机的情况

// 对于漫反射材质，表面越黑，吸收光的能力越强。任何的随机方向算法都会让表面看起来哑光。
// 最简单的方式是实现哑光最正确的方式！
// 使用兰伯特反射其实在实现哑光效果上是不正确的。

// 如何确定漫反射发生光线反射的方向：在和表面接触点相切的单位球内随机采样一个点。与表面接触点构成的向量方向即为光线反射方向
// 在球体随机选择一个点可以使用拒绝方法，（但这是非均匀采样）

// material determine when ray interact with sphere , what happen next

#include "ray.h"
#include "object.h"

struct hit_record;

// base class of material
class material {
public:
	// scatter 存储了当前光信息，碰撞点信息，散射点颜色，散射后的光信息
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const = 0;

};

// modelling diffuse material
class lambertian :public material {
public:
	lambertian(const color & a) : albedo(a){}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		auto scatter_direction = rec.normal + random_unit_vector();
		// if rec.normal = - random unit vetor, compute error, so judge it
		if (scatter_direction.near_zero()) scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true; 
	}

public:
	color albedo;

};


// return the result of mirror reflection
vec3 reflect(const vec3& ray_in, const vec3& normal)
{
	return ray_in - 2 * dot(ray_in, normal) * normal;
}


float reflectance(const float& cos_theta, const float& ref_ratio)
{
	// Use Schlick's approximation for reflectance. Schlick近似
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





// 金属的反射类型是镜面反射类型，因此需要确定光线经过镜面反射得到的反射光线方向
// modelling metal material
class metal :public material {
public:
	metal(const color& a, float f):albedo(a),fuzz(f<1?f:1) {}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		// 镜面反射结果计算
		vec3 reflected = reflect(normalized(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere()); // fuzz & random_in_unit_sphere控制相关区域毛躁效果
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0.0); // 如果值小于0，那么就是在球体内，认为没有出现相交
	}


// Fuzzy Reflection,金属材质的毛躁程度相关，对应终端光线的小范围波动
public:
	color albedo;
	float fuzz;
};



// 介质，对应透明材质， 在透明材质中，一部分光线会在介质中传播，进行透射
// when a light ray hits them, it split into a reflected ray and a fracted ray. 
// reflection and refraction
// 兰伯特反射，斯涅尔定律: 根据夹角和对应的介质折射率确定折射角的方向
// 常见介质的折射率：typically air = 1.0, glass = 1.3 ~ 1.7 diamond = 2.4. sintheta1 * ita1= sintheta2 * ita2 
class dielectric :public material {
public:
	dielectric(float index_of_refraction): ir(index_of_refraction) {}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		attenuation = color(1.0f, 1.0f, 1.0f);
		float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		vec3 unit_direction = unit_vector(r_in.direction());
		vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);
		scattered = ray(rec.p, refracted);
		return true;
	}

	// 真实的玻璃其折射光的折射量随着角度的变化会发生变化，所以我们需要对真实的折射率施加简单的变化公式
public:
	float ir;
};



