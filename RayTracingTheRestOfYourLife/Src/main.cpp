#pragma once

#include "rtweekend.h" 
#include "color.h"
#include "object.h"
#include "hittable_list.h"
#include "camera.h"
#include "ray.h"
#include "material.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include "pdf.h"

#include <iostream>
#include <fstream>
#include <iomanip>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// a simple brute force path tracer 光线追踪一是一个暴力的路径追踪求解器

hittable_list cornell_box()
{
	hittable_list objects;
	// this scence is deplicated from cornell_box simple
	// ray is random form light so the scene is noisy, 递归到光源的光停止，表现为光源的光线加噪声形成
	auto red = make_shared<lambertian>(color(.65f,.05f,.05f));
	auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
	auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
	auto light = make_shared<diffuse_light>(color(15,15,15));
	
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light))); // light set
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	auto aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);  // 要使用基类指针，否则无法通过虚函数表进行多态
	box1 = make_shared<y_rotate>(box1, 15); // use euler angle to denote rotation , y- axis
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);
	
	/*
	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<y_rotate>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);
	*/
	auto glass = make_shared<dielectric>(1.5);
	objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

	return objects;
};


color ray_color(const ray &r,const color& background, const hittable_list& world,shared_ptr<hittable_list>& lights,int depth)
{
	hit_record rec_info;
	if (depth <= 0)
		return color(0, 0, 0); // if exceed ray depth and not reflect by environment return
	// using 0.001 rather than 0.0 means ignore float precision error
	
	if (!world.hit(r, 0.001, infinity, rec_info))
	{
		return background;
	}
	
	// how about return info, add emit light and normal light 
	scatter_record srec;
	color emitted = rec_info.mat_ptr->emitted(r,rec_info, rec_info.u, rec_info.v, rec_info.p);
	
	if (!rec_info.mat_ptr->scatter(r, rec_info, srec)) // 
		return emitted;


	if (srec.is_specular) {
		return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, depth - 1);
	}

	auto light_ptr = make_shared<hittable_pdf>(lights, rec_info.p);
	// if (light_ptr->value( srec.specular_ray()) < 0.0000001f) return emitted;
	mixture_pdf pdf(light_ptr, srec.pdf_ptr);
	

	ray scattered = ray(rec_info.p, pdf.generate(), r.time());
	float pdf_val = pdf.value(scattered.direction());
	if (pdf_val < 0.001) return emitted;
	// ==============================================
	
	return emitted + srec.attenuation * rec_info.mat_ptr->scattering_pdf(r, rec_info, scattered) * ray_color(scattered, background, world, lights, depth - 1) / pdf_val; // A * S * Color / PDF
}


int main() {
	// ====Define Image(Screen) and Camera====
	float aspect_ratio = 16.0f / 9.0f;
	int image_width = 1000; 
	int image_height = static_cast<int>(image_width / aspect_ratio);
	int samples_per_pixel = 1000;
	int max_depth = 50; 
	color background(0, 0, 0);
	
	// ====Define World Scene====
	hittable_list world;
	//====Set Camera position and property====
	point3 lookfrom = point3(13.0f, 2.0f, 3.0f);
	point3 lookat = point3(0.0f, 0.0f, 0.0f);
	vec3 vup = vec3(0.0f, 1.0f, 0.0f);
	float fov_degree = 20.0f;
	float aperture = 0.1f;
	float focus_dist = 10.0f;
	//shared_ptr<hittable> lights = make_shared<xz_rect>(213, 343, 227, 332, 554, make_shared<material>()); // lights info only use for pdf compute
	//shared_ptr<hittable> lights = make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>());
	shared_ptr<hittable_list> lights = make_shared<hittable_list>();
	lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, make_shared<material>())); // lights info only use for pdf compute
	lights->add(make_shared<sphere>(point3(190,90,190), 90, shared_ptr<material>()));
	
	world = cornell_box();
	aspect_ratio = 1.0;
	image_width = 1000;
	image_height = static_cast<int>(image_width / aspect_ratio);
	background = color(0.0, 0.0, 0.0);
	lookfrom = point3(278, 278, -800);
	lookat = point3(278, 278, 0);
	fov_degree = 40.0f;
	aperture = 0.0f;

	camera cam(lookfrom, lookat, vup, fov_degree, aspect_ratio, aperture, focus_dist,0.0, 1.0); // camera has the same shutter time with ball move
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
				float v = float(j + random_float()) / float(image_height - 1); 
				ray r = cam.get_ray(u, v);
			
				pixel_color += ray_color(r, background, world, lights ,max_depth);
			}
			
			write_color(out_txt_file, pixel_color, samples_per_pixel);
		}
	}
	out_txt_file.close();
	std::cerr << "\nDone.\n";

	return 0;
}




/*
// Marto Carlo Integration
int main() {
	int inside_circle = 0;
	int inside_circle_stratified = 0;

	int sqrt_N = 10000;


	long long runs = 0;
	std::cout << std::fixed << std::setprecision(12);
	while (true) {
		runs++;
		auto x = random_float(-1, 1); // 浮点数精度有限！！
		auto y = random_float(-1, 1);
		if (x * x + y * y < 1)
			inside_circle++;

		if (runs % 100000 == 0)
			std::cout << "Estimate of Pi = "
			<< 4 * double(inside_circle) / runs
			<< '\n';
	}
	// 不会在本书中分层抖动
}
*/
/*
一维蒙特卡洛积分方法（面积估计法）
*/