#pragma once

#include "rtweekend.h"
#include "vec3.h"
// Let’s just use some sort of hashing to scramble this, instead of tiling. This has a bit of support code to make it all happen:
// Perlin Noise Templete ： How to achieve this templete?
// Ken Perlin , a clever man !


class perlin {
public:
	perlin() {
		ranvec = new vec3[point_count];
		for (int i = 0; i < point_count; i++)
		{
			ranvec[i] = unit_vector(vec3::random(-1, 1));
		}
		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();

	}

	~perlin() {
		delete ranvec;
		delete perm_x;
		delete perm_y;
		delete perm_z;
	}

	// little error for weight
	float noise(const point3& p) const {
		auto i = static_cast<int>(4 * p.x()) & 255;
		auto j = static_cast<int>(4 * p.y()) & 255;
		auto k = static_cast<int>(4 * p.z()) & 255;
		vec3 weight_v(i, j, k);
		float res = dot(ranvec[perm_x[i] ^ perm_y[j] ^ perm_z[k]], weight_v) / sqrtf(3.0f);
		return res;
	}


	float noise_smooth(const point3& p) const {
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());

		

		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}

		return trilinear_interp(c, u, v, w);
	}


	float noise_hermitian(const point3& p) const {
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());
		u = u * u * (3 - 2 * u);
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}

		return trilinear_interp(c, u, v, w);
	}

	// turbulence 湍流类
	float turb(const point3& p, int depth = 7) const {
		float accum = 0.0f;
		point3 temp_p = p;
		float weight = 1.0f;

		// 七级湍流
		for (int i = 0; i < depth; i++) {
			accum += weight * noise_hermitian(temp_p);
			weight *= 0.5f;
			temp_p *= 2;
		}

		return fabs(accum);
	}



private:
	static const int point_count = 256; // 采样点数
	vec3* ranvec;
	int* perm_x;
	int* perm_y;
	int* perm_z;


	static int* perlin_generate_perm()
	{
		auto p = new int[point_count];

		for (int i = 0; i < perlin::point_count; i++) p[i] = i;

		permute(p, point_count);

		return p;

	}


	static void permute(int* p, int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			int target = random_int(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}


	static float trilinear_interp(vec3 c[2][2][2], float u, float v, float w)
	{
		// 三线性插值，重心坐标插值公式
		float accum = 0.0f;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					vec3 weight = vec3(u - i, v - j, k - w);
					accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w)) * dot(c[i][j][k], weight);
				}

		return accum;
	}

};


// before chapter5.5
/*
class perlin {
public:
	perlin() {
		ranfloat = new float[point_count];
		for (int i = 0; i < point_count; i++)
		{
			ranfloat[i] = random_float();
		}
		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();

	}

	~perlin() {
		delete ranfloat;
		delete perm_x;
		delete perm_y;
		delete perm_z;
	}

	float noise(const point3& p) const {
		auto i = static_cast<int>(4 * p.x()) & 255;
		auto j = static_cast<int>(4 * p.y()) & 255;
		auto k = static_cast<int>(4 * p.z()) & 255;

		return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}


	float noise_smooth(const point3& p) const {
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());

		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		float c[2][2][2];

		for ( int di=0; di<2; di++)
			for(int dj=0; dj<2; dj++)
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = ranfloat[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}

		return trilinear_interp(c, u, v, w);
	}


	float noise_hermitian(const point3& p) const {
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());
		u = u * u * (3 - 2 * u);
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		float c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = ranfloat[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
				}

		return trilinear_interp(c, u, v, w);
	}


		
private:
	static const int point_count = 256; // 采样点数
	float* ranfloat;
	int* perm_x;
	int* perm_y;
	int* perm_z;


	static int* perlin_generate_perm()
	{
		auto p = new int[point_count];

		for (int i = 0; i < perlin::point_count; i++) p[i] = i;

		permute(p, point_count);

		return p;

	}


	static void permute(int* p, int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			int target = random_int(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}


	static float trilinear_interp(float c[2][2][2], float u, float v, float w)
	{
		// 三线性插值，重心坐标插值公式
		float accum = 0.0f;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					accum += (i*u + (1-i) * (1-u) ) * (j*v + (1-j) * (1-v)) * (k*w + (1-k) * (1-w)) * c[i][j][k];
		
		return accum;
	}

};
*/


