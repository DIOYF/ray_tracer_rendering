#pragma once

#include "color.h"
#include "object.h"
#include "rtweekend.h" 
#include "hittable_list.h"
#include "camera.h"
#include "ray.h"
#include "material.h"

#include <iostream>
#include <fstream>
#include <iomanip>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;


hittable_list random_scene()
{
	hittable_list world;
	// this scence is deplicated from raytracing in one weekend
	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_float();
			point3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_float(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;

};





color ray_color(const ray &r, const hittable_list& world, int depth)
{
	hit_record rec_info;
	//float t = hit_sphere(sphere_center, sphere_radius, r); // ��⵽��Ӧ����ײ��
	if (depth <= 0)
		return color(0, 0, 0); // if exceed ray depth and not reflect by environment return
	// using 0.001 rather than 0.0 means ignore float precision error
	if (world.hit(r,0.001, infinity, rec_info))
	{
		// how about return info
		ray scattered;
		color attenuation;
		if (rec_info.mat_ptr->scatter(r, rec_info, attenuation, scattered)) // �ཻ��Ϣ��Զ���ݵ�rec_info��
			return attenuation * ray_color(scattered, world, depth-1); // ��ɫ��Ϣͨ��attenuation��˵���,ÿ�����߶���Ҫ�ͳ����ж�
		return color(0, 0, 0);
		/*
		// vec3 Target = (Position + Normal) + random_in_unit_sphere(); ע����Ȳ�������Ӱ�������ԣ������
		//vec3 Target = (Position+ Normal) + random_in_hemisphere(Normal);
		vec3 Target = (Position + Normal) + random_unit_vector(); // Many different method
		
		return 0.5 * ray_color(ray(Position ,Target - Position), world, depth-1); // using once hit result 
		*/
		// we can use recursion algorithm and determine the ray bounce numbers with depth  
	}

	vec3  unit_direction = unit_vector(r.direction());
	color startValue = { 0.5294f, 0.8078f, 0.9803f };
	color endValue = {1.f, 1.f, 1.f};
	float alpha = (unit_direction.y() + 1.0f) / 2.0f;
	return startValue * alpha + endValue * (1-alpha);
}


int main() {
	// ====Define Image(Screen) and Camera====
	const float aspect_ratio = 4.0f / 3.0f;
	const int image_width = 400; // high performance set 1920
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 100;  // high performance set 800
	const int max_depth = 50; // ���ߵ���50��

	// ====Define World Scene====
	hittable_list world = random_scene();
	/*
	shared_ptr<lambertian> sphere_1 = make_shared<lambertian>(color(0.7f,0.2f,0.2f));
	shared_ptr<lambertian> sphere_2 = make_shared<lambertian>(color(0.8f,0.8f,0.0f));
	shared_ptr<metal> sphere_3 = make_shared<metal>(color(0.6f, 0.6f, 0.6f),0.1); // adjust parameter
	shared_ptr<metal> sphere_4 = make_shared<metal>(color(0.8f, 0.6f, 0.2f),0.3);
	shared_ptr<dielectric> sphere_5 = make_shared<dielectric>(1.6);

	world.add(make_shared<sphere>(point3(0.f,0.0f,-1.0f), 0.5f, sphere_1));
	world.add(make_shared<sphere>(point3(0.f,-100.5f, -1.0f), 100.f, sphere_2)); // big ball as ground
	world.add(make_shared<sphere>(point3(-1.0f, 0.0f, -1.0f), 0.5f, sphere_3)); // 
	//world.add(make_shared<sphere>(point3(1.0f, 0.0f, -1.0f), 0.5f, sphere_4)); //
	world.add(make_shared<sphere>(point3(1.0f, 0.0f, -1.0f), 0.5f, sphere_5));
	*/

	/*
	// ====Set Camera position and property====
	float viewport_height = 2.0f;
	float viewport_width = aspect_ratio * viewport_height;
	float focal_length = 1.0f; // not focus_distance , it means projection plane and projection point to be one unit
	point3 origin = point3(0, 0, 0);
	vec3 horizontal = vec3(viewport_width, 0, 0);
	vec3 vertical = vec3(0, viewport_height, 0);
	point3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length); // lower_left_corner position
	*/
	// look_from,look_at,vup,fov


	// ������Ժ��������
	point3 lookfrom = point3(13.0f, 2.0f, 3.0f);
	point3 lookat = point3(0.0f, 0.0f, 0.0f);
	vec3 vup = vec3(0.0f, 1.0f, 0.0f);
	float fov_degree = 20.0f;
	float aperture = 0.1f; // ��Ȧ��С0~5�ȽϺ���
	float focus_dist = 10.0f;


	camera cam(lookfrom, lookat, vup, fov_degree, aspect_ratio,aperture, focus_dist);

	// ====Render to one ppm format image====
	std::ofstream out_txt_file;
	out_txt_file.open("./image_text.ppm", std::ios::out | std::ios::trunc);
	out_txt_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	

	//The pixels are written out in rows with pixels left to right. The rows are written out from top to bottom.
	for (int j = image_height-1; j>= 0; j--)
	{
		// add a progress indicator to output
		std::cerr << "\rScanlines remaining:" << j << ' ' << std::flush;
		for (int i = 0; i < image_width; i++)
		{
			// move ray to determine pixel_color
			color pixel_color = color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++)
			{
				float u = float(i + random_float()) / float(image_width - 1);
				float v = float(j + random_float()) / float(image_height - 1); // ȷ����Ļ�ռ䣨image resolution��������ռ� ( view port)֮��ĳ����ȣ�
				ray r = cam.get_ray(u, v);
				// dirty process method : �������������ֵ������100�β�������������dirty work�� dont reproduction(ÿ��λ�ö�������ȡƽ��)
				pixel_color += ray_color(r, world, max_depth);
			}
			
			write_color(out_txt_file, pixel_color, samples_per_pixel);

			/*
			{
				auto r = float(i) / (image_width - 1.0f);
				auto g = float(j) / (image_height - 1.0f);
				auto b = 0.25f;

				int ir = static_cast<int>(255.999f * r); // static force type cast from float to cast
				int ig = static_cast<int>(255.999f * g);
				int ib = static_cast<int>(255.999f * b);
				out_txt_file << ir << ' ' << ig << ' ' << ib << '\n';
			}
			*/	
		}
	}
	out_txt_file.close();
	std::cerr << "\nDone.\n";

	return 0;
}