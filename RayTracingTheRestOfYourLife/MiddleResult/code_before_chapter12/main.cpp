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



hittable_list two_spheres()
{
	hittable_list objects;
	auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9,0.9,0.9));
	auto pertext = make_shared<noise_texture>(4.0);
	//objects.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));
	//objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	objects.add(make_shared<sphere>(point3(0,  -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	return objects;
}


hittable_list earth()
{
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	return hittable_list(globe);
}

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

			//Each small sphere moves from its center Cat time t = 0 to C + (0, r / 2, 0)at time t = 1, where ris a random number in[0, 1)

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse material add motion blur (using random ray time parameter)
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					point3 center2 = center + point3(0.0, random_float(0.0, 0.5), 0.0);
					world.add(make_shared<moving_sphere>(center, center2,0.0, 1.0, 0.2, sphere_material));
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

hittable_list simple_light()
{
	hittable_list objects;
	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3,5,1,3,-2,difflight));

	return objects;
};

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

	
	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);  // 要使用基类指针，否则无法通过虚函数表进行多态
	box1 = make_shared<y_rotate>(box1, 15); // use euler angle to denote rotation , y- axis
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);
	
	
	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<y_rotate>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);

	return objects;
};



hittable_list cornell_smoke()
{
	hittable_list objects;
	// this scence is deplicated from cornell_box simple
	// ray is random form light so the scene is noisy, 递归到光源的光停止，表现为光源的光线加噪声形成
	auto red = make_shared<lambertian>(color(.65f, .05f, .05f));
	auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
	auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));


	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);  // 要使用基类指针，否则无法通过虚函数表进行多态
	box1 = make_shared<y_rotate>(box1, 15); // use euler angle to denote rotation , y- axis
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	box1 = make_shared<constant_medium>(box1,0.01, color(0,0,0));
	objects.add(box1);


	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<y_rotate>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	box2 = make_shared<constant_medium>(box2, 0.01, color(1,1,1));
	objects.add(box2);

	return objects;
};



// final scene use bvh to accelate
hittable_list final_scene() {
	// many objects, using bvh to speed up

	// green ground
	hittable_list boxes1;
	auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));
	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			float w = 100.0;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0.0;
			
			float x1 = x0 + w;
			float y1 = random_float(1, 101);
			float z1 = z0 + w;

			boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}

	hittable_list objects;
	objects.add(make_shared<bvh_node>(boxes1, 0, 1));
	
	// emit rectangle light
	auto light = make_shared<diffuse_light>(color(7, 7, 7));
	objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light) );
	
	// add moving sphere
	point3 center1 = point3(400, 400, 200);
	point3 center2 = center1 + vec3(30, 0, 0);
	auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
	objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

	// some ball like rtone, metal , dielectric
	objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
	objects.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

	// volume flog ball with dielectric material , hit sphere ,hit constant_medium , get last result , cool code mode
	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
	
	objects.add(boundary);  // inital boundary
	objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9) )); // add flog volume
	
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
	objects.add(make_shared<constant_medium>(boundary, 0.0001, color(1, 1, 1))); // 泛光效果

	// earth image texture
	auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
	objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
	
	// perlin noise
	auto pertext = make_shared<noise_texture>(0.1);
	objects.add(make_shared<sphere>(point3(200,280,300), 100, make_shared<lambertian>(pertext)) );
	

	hittable_list boxes2;
	auto white = make_shared<lambertian>( color(.73, .73, .73));
	int ns = 1024; // 1024 with bvh o(log(1024)) = 10
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white ));
	}
	
	objects.add(make_shared<translate>( make_shared<y_rotate>( make_shared<bvh_node>(boxes2, 0.0, 1.0) ,15), vec3(-100, 270 , 395)));


	return objects;
}




color ray_color(const ray &r,const color& background, const hittable_list& world,shared_ptr<hittable>& lights,int depth)
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
	ray scattered;
	float pdf;
	color attenuation;
	color emitted =  rec_info.mat_ptr->emitted(r, rec_info, rec_info.u, rec_info.v, rec_info.p);
	
	if (!rec_info.mat_ptr->scatter(r, rec_info, attenuation, scattered, pdf)) // 
		return emitted;

	/*
	cosine_pdf pdf_info(rec_info.normal);
	scattered = ray(rec_info.p, pdf_info.generate(), r.time());
	pdf = pdf_info.value(scattered.direction()); // 球面依概率采样
	*/

	/*
	hittable_pdf light_pdf(lights, rec_info.p); // 对光源进行额外的建模
	scattered = ray(rec_info.p, light_pdf.generate(), r.time());
	pdf = light_pdf.value(scattered.direction());
	*/

	
	auto pdf_info = make_shared<cosine_pdf>(rec_info.normal);
	auto light_pdf = make_shared<hittable_pdf>(lights, rec_info.p);
	mixture_pdf mixed_pdf(pdf_info, light_pdf);
	scattered = ray(rec_info.p, mixed_pdf.generate(), r.time());
	pdf = mixed_pdf.value(scattered.direction());
	if (pdf < 0.0001) return emitted;
	
	
	/*
	// ============= hard code light, sampling more light from area light ==============
	point3 on_light = point3(random_float(213, 343), 554, random_float(227, 332));
	point3 to_light = on_light - rec_info.p;
	float distance_squared = to_light.length_squared();
	to_light = unit_vector(to_light);

	if (dot(to_light, rec_info.normal) < 0) return emitted; // 光源和法线夹角超过90度

	float light_area = (343 - 213) * (332 - 227);
	float light_cosine = fabs(to_light.y());
	if (light_cosine < 0.0000001) // 几乎水平于光源方向
		return emitted;
		
	pdf = distance_squared / (light_cosine * light_area); // chapter9 P(direction) = D^2(p,q) / (COS(alpha) * A)
	scattered = ray(rec_info.p, to_light, r.time());
	*/


	// ==============================================
	return emitted + attenuation * rec_info.mat_ptr->scattering_pdf(r, rec_info, scattered) * ray_color(scattered, background, world, lights, depth - 1) / pdf; // A * S * Color / PDF
	


	/*
	// how to define background color? (sky blue) , or we use emit object
	vec3  unit_direction = unit_vector(r.direction());
	color startValue = { 0.5294f, 0.8078f, 0.9803f };
	color endValue = {1.f, 1.f, 1.f};
	float alpha = (unit_direction.y() + 1.0f) / 2.0f;
	return startValue * alpha + endValue * (1-alpha);
	*/
}


int main() {
	// ====Define Image(Screen) and Camera====
	float aspect_ratio = 16.0f / 9.0f;
	int image_width = 400; 
	int image_height = static_cast<int>(image_width / aspect_ratio);
	int samples_per_pixel = 10;  
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
	shared_ptr<hittable> lights = (make_shared<xz_rect>(213, 343, 227, 332, 554, make_shared<diffuse_light>(color(7, 7, 7)))); // lights info only use for pdf compute
	switch (5)
	{
	case 1:
		world = random_scene();
		background = color(0.70, 0.80, 1.00);
		lookfrom = point3(13.0f, 2.0f, 3.0f);
		lookat = point3(0, 0, 0);
		fov_degree = 20.0f;
		aperture = 0.1f;
		break;

	default:
	case 2:
		world = two_spheres();
		background = color(0.70, 0.80, 1.00);
		lookfrom = point3(13.0f, 2.0f, 3.0f);
		lookat = point3(0, 0, 0);
		fov_degree = 20.0f;
		aperture = 0.0f;
		break;

	case 3:
		world = earth();
		lookfrom = point3(13.0f, 2.0f, 3.0f);
		background = color(0.70, 0.80, 1.00);
		lookat = point3(0, 0, 0);
		fov_degree = 20.0f;
		aperture = 0.0f;
		break;


	case 4:
		world = simple_light();
		background = color(0.0, 0.0, 0.0);
		samples_per_pixel = 400;
		lookfrom = point3(26, 3, 6);
		lookat = point3(0, 2, 0);
		fov_degree = 20.0f;
		aperture = 0.0f;
		
		break;


	case 5:
		world = cornell_box();
		aspect_ratio = 1.0;
		image_width = 600;
		image_height = static_cast<int>(image_width / aspect_ratio);
		background = color(0.0, 0.0, 0.0);
		samples_per_pixel = 100;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		fov_degree = 40.0f;
		aperture = 0.0f;
		break;

	case 6:
		world = cornell_smoke();
		aspect_ratio = 1.0;
		image_width = 600;
		background = color(0.0, 0.0, 0.0);
		image_height = static_cast<int>(image_width / aspect_ratio);
		samples_per_pixel = 200;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		fov_degree = 40.0f;
		aperture = 0.0f;
		break;

	case 7:
		world = final_scene();
		aspect_ratio = 1.0;
		image_width = 1000;
		image_height = static_cast<int>(image_width / aspect_ratio);
		samples_per_pixel = 1000;
		background = color(0, 0, 0);
		lookfrom = point3(478, 278, -600);
		lookat = point3(278, 278, 0);
		fov_degree = 40.0f;
		aperture = 0.0f;
		break;
	}

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