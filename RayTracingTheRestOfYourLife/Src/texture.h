#pragma once

//Solid Texture, makes the colors on a surface procedural
#include "rtweekend.h"
#include "vec3.h"
#include "perlin.h"
#include "rtw_stb_image.h"
#include <iostream>

class texture {
public:
	virtual color value(float u, float v, const point3& p) const = 0;
};


class solid_color : public texture{
public:
	solid_color(){}
	solid_color(color c): color_value(c) {}

	solid_color(float red, float green, float blue)
		: solid_color(color(red,green,blue)){}

	virtual color value(float u, float v, const point3& p) const override{
		return color_value;
	}
private:
	color color_value;
};


class checker_texture : public texture {
public:
	checker_texture(){}
	checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd) : even(_even), odd(_odd){}
	checker_texture(color c1, color c2) : even(make_shared<solid_color>(c1)),odd(make_shared<solid_color>(c2)){}

	virtual color value(float u, float v, const point3& p) const override {
		auto sines = sin(10* p.e[0]) * sin(10 * p.e[1]) * sin(10 * p.e[2]);
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	} // 纹理采样


public:
	// texture add to a checker
	shared_ptr<texture> odd;
	shared_ptr<texture> even;
};

// perlin noise texture
class noise_texture : public texture {
public:
	noise_texture() { scale = 1.0f; }
	noise_texture(float _scale) : scale(_scale) {}
	virtual color value(float u, float v, const point3& p) const override {
		// for float random
		//return color(1, 1, 1) * noise.noise(p);
		//return color(1,1,1) * noise.noise_smooth(p);
		//return color(1, 1, 1) * noise.noise_hermitian(scale * p);
		// for vec3 random
		//return color(1, 1, 1) * 0.5 * (1.0 + noise.noise_hermitian(scale * p));
		// san DOWN !!!!!
		//return color(1, 1, 1) * noise.turb(scale * p);
		return color(1, 1, 1) * 0.5 * ( 1 + sin(scale * p.z() + 10 * noise.turb(p) ) );

	}

public:
	perlin noise;
	float scale;
};



//2D image texture mapping , texture sampping
//从纹理资源文件中导入texture
class image_texture : public texture {
public:
	const static int bytes_per_pixel = 3; // RGB 255 texture
	image_texture() : data(nullptr), width(0), height(0){}
	image_texture(const char* filename) {
		auto components_per_pixel = bytes_per_pixel;
		data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

		if (!data) {
			std::cerr << "Can not load image file" << filename << std::endl;
			width = height = 0;
		}

		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture()
	{
		delete  data;
	}


	virtual color value(float u, float v, const point3& p) const override {
		if (data == nullptr) return color(0.0f, 1.0f, 1.0f);

		u = clamp(u, 0.0f, 1.0f);
		v = 1 - clamp(v, 0.0f, 1.0f);

		// sampleing
		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);
	
		const auto color_scale = 1.0 / 255.0;

		auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;

};




