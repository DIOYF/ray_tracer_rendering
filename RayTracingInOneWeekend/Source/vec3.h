#pragma once

#include "rtweekend.h"
#include <cmath>
#include <iostream>

using std::sqrt;
using std::shared_ptr;
using std::make_shared;


class vec3 {
public:
	vec3() : e{ 0,0,0 } {}
	vec3(float e0, float e1, float e2) :e{ e0,e1,e2 } {}

	float x() { return e[0]; }
	float y() { return e[1]; }
	float z() { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } // ����������Ȩ��ȡ�ⲿ���ݣ�����Ȩ�޸�����
	float operator[](int i) const { return e[i]; } // why return normal with const
	float& operator[](int i) { return e[i]; }   // why return & with not const

	vec3& operator+=(const vec3& v) {
		e[0] += v[0];
		e[1] += v[1];
		e[2] += v[2];
		return *this;
	}

	vec3& operator-=(const vec3& v) {
		e[0] -= v[0];
		e[1] -= v[1];
		e[2] -= v[2];
		return *this;
	}

	vec3& operator*=(const float t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
	}

	vec3& operator/=(const float t) {
		e[0] /= t;
		e[1] /= t;
		e[2] /= t;
	}

	// ʵ������������㷨, ע�������Ǿ�̬�෽��
	inline static vec3 random() {
		return vec3(random_float(), random_float(), random_float());
	}

	inline static vec3 random(float min, float max) {
		return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
	}

	bool near_zero() const {
		const float epsilon = 1e-8;
		return (fabs(e[0]) < epsilon)&& (fabs(e[1]) < epsilon) &&(fabs(e[2]) < epsilon);
	}

	float length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	float length() const {
		// return the length of the vector
		return sqrt(length_squared());
	}


public:
	float e[3];
};

// Type aliases
using point3 = vec3;
using color = vec3;



// vec3 Utility Functions, Ϊ��Ӧ���ඨ���µ���չ
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) { // �������ò����ٵ�����ĳ�Ա������
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}


inline vec3 operator-(const vec3& u, const vec3& v) { // �������ò����ٵ�����ĳ�Ա������
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) { // �������ò����ٵ�����ĳ�Ա������
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}


inline vec3 operator*(const vec3& u, const float t) { // �������ò����ٵ�����ĳ�Ա������
	return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t);
}

inline vec3 operator*(const float t ,const vec3& u) { // �������ò����ٵ�����ĳ�Ա������
	return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t);
}

inline vec3 operator/(const vec3 &v, const float t) {
	return (1 / t) * v;
}

inline float dot(const vec3 &u, const vec3 &v) {
	return u.e[0]*v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u,const  vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		        u.e[2] * v.e[0] - u.e[0] * v.e[2],
		        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// note to change unit_vector to normalized
inline vec3 normalized(vec3 v) {
	return v / v.length();
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

// such method to sample means result close to normal with more probablity which is suitable
// using Lambertian distribution, which has a distribution of cos(phi)
vec3 random_in_unit_sphere()
{
	while (true) {
		// now origin is (0,0,0)
		vec3 p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

vec3 random_unit_vector() 
{
	return unit_vector(random_in_unit_sphere()); // makes the sampling more uniform
}

vec3 random_in_hemisphere(const vec3& normal)
{
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) return in_unit_sphere;
	else return -in_unit_sphere;

}

// generate a random disk to get defocus blur
vec3 random_in_unit_disk() 
{
	while (true) {
		auto p = vec3(random_float(-1,1), random_float(-1, 1), 0);
		if (p.length_squared() < 1.0) return p;
	}
}