#pragma once

// ��������Ŀǰ���������ķ��򣬷�������ɫ�������ԣ���Ӧ����Ϣ��¼��scattered

// ���Է�����������ʵ�������ʾ����������Χ����ɫ�����������������ɫҲ���������ɫ���е��ƣ�module��
// ��������ʵ����巴���ķ���������ģ����ǵķ�������ʾ��ȫ��������

// ������������ʣ�����Խ�ڣ����չ������Խǿ���κε���������㷨�����ñ��濴�����ƹ⡣
// ��򵥵ķ�ʽ��ʵ���ƹ�����ȷ�ķ�ʽ��
// ʹ�������ط�����ʵ��ʵ���ƹ�Ч�����ǲ���ȷ�ġ�

// ���ȷ�������䷢�����߷���ķ����ںͱ���Ӵ������еĵ�λ�����������һ���㡣�����Ӵ��㹹�ɵ���������Ϊ���߷��䷽��
// ���������ѡ��һ�������ʹ�þܾ��������������ǷǾ��Ȳ�����

// material determine when ray interact with sphere , what happen next

#include "ray.h"
#include "object.h"

struct hit_record;

// base class of material
class material {
public:
	// scatter �洢�˵�ǰ����Ϣ����ײ����Ϣ��ɢ�����ɫ��ɢ���Ĺ���Ϣ
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
	// Use Schlick's approximation for reflectance. Schlick����
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





// �����ķ��������Ǿ��淴�����ͣ������Ҫȷ�����߾������淴��õ��ķ�����߷���
// modelling metal material
class metal :public material {
public:
	metal(const color& a, float f):albedo(a),fuzz(f<1?f:1) {}
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		// ���淴��������
		vec3 reflected = reflect(normalized(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere()); // fuzz & random_in_unit_sphere�����������ë��Ч��
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0.0); // ���ֵС��0����ô�����������ڣ���Ϊû�г����ཻ
	}


// Fuzzy Reflection,�������ʵ�ë��̶���أ���Ӧ�ն˹��ߵ�С��Χ����
public:
	color albedo;
	float fuzz;
};



// ���ʣ���Ӧ͸�����ʣ� ��͸�������У�һ���ֹ��߻��ڽ����д���������͸��
// when a light ray hits them, it split into a reflected ray and a fracted ray. 
// reflection and refraction
// �����ط��䣬˹��������: ���ݼнǺͶ�Ӧ�Ľ���������ȷ������ǵķ���
// �������ʵ������ʣ�typically air = 1.0, glass = 1.3 ~ 1.7 diamond = 2.4. sintheta1 * ita1= sintheta2 * ita2 
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

	// ��ʵ�Ĳ��������������������ŽǶȵı仯�ᷢ���仯������������Ҫ����ʵ��������ʩ�Ӽ򵥵ı仯��ʽ
public:
	float ir;
};



