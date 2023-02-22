#pragma once

#include "vec3.h"
#include "rtweekend.h"
#include <iostream>
#include <fstream>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel ){
	float r = pixel_color.x();
	float g = pixel_color.y();
	float b = pixel_color.z();
	
	auto scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;

	// Write the translated [0, 255] value of each color 
	out << static_cast<int>(256.0f * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256.0f * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256.0f * clamp(b, 0.0, 0.999)) << '\n';
}


void write_color(std::ofstream& out, color pixel_color, int samples_per_pixel) {
	float r = pixel_color.x();
	float g = pixel_color.y();
	float b = pixel_color.z();
	/*
	auto scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;
	*/
	// gamma correction for accurate color intensity 
	float gamma = 2.0f;
	auto scale = 1.0f / samples_per_pixel;
	r = powf( r * scale, 1.0f / gamma);
	g = powf( g * scale, 1.0f / gamma);
	b = powf( b * scale, 1.0f / gamma);


	// Write the translated [0, 255] value of each color 
	out << static_cast<int>(256.0f * clamp(r, 0.0, 0.999))<<' '
		<< static_cast<int>(256.0f * clamp(g, 0.0, 0.999))<<' '
		<< static_cast<int>(256.0f * clamp(b, 0.0, 0.999))<<'\n';
}


