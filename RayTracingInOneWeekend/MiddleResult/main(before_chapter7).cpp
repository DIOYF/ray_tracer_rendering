#include "color.h"
#include "object.h"
#include "rtweekend.h" 
#include "hittable_list.h"

#include <iostream>
#include <fstream>
#include <iomanip>


color ray_color(const ray &r, const hittable_list& world)
{
	hit_record rec_info;
	//float t = hit_sphere(sphere_center, sphere_radius, r); // 检测到对应的碰撞点
	if (world.hit(r,0.0,10.0, rec_info))
	{
		vec3 Normal = rec_info.normal;
		vec3 ray_direction = r.direction();
		return 0.5 * color( Normal.x() + 1, Normal.y() + 1, Normal.z() + 1 );
	}
	color startValue = { 0.5294f, 0.8078f, 0.9803f };
	color endValue = {1.f, 1.f, 1.f};
	float alpha = (r.at(1.0)[1] + 1.0f) / 2.0f;
	return startValue * alpha + endValue * (1-alpha);
}


int main() {
	// ====Define Image(Screen) and Camera====
	const float aspect_ratio = 16.0f / 9.0f;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	// ====Define World Scene====
	hittable_list world;
	world.add(make_shared<sphere>(point3(0.f,0.1f,-1.0f), 0.3f));
	world.add(make_shared<sphere>(point3(0.f,-100.5f, -1.0f), 100.0f)); // big ball as ground


	// ====Set Camera position and property====
	float viewport_height = 2.0f;
	float viewport_width = aspect_ratio * viewport_height;
	float focal_length = 1.0f; // not focus_distance , it means projection plane and projection point to be one unit
	point3 origin = point3(0, 0, 0);
	vec3 horizontal = vec3(viewport_width, 0, 0);
	vec3 vertical = vec3(0, viewport_height, 0);
	point3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length); // lower_left_corner position


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
			float u = float(i) / float(image_width-1);
			float v = float(j) / float(image_height-1); // 确定屏幕空间（image resolution）和相机空间 ( view port)之间的长宽比，
			ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin );

			color pixel_color = ray_color(r, world);
			write_color(out_txt_file, pixel_color);

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