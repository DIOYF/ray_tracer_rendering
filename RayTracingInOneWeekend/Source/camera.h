#pragma once

#include "rtweekend.h"
#include "ray.h"


using std::shared_ptr;
using std::make_shared;
using std::sqrt;
// using a camera with MSAA method
// we just exlore camera position, camera horizontal, camera vertical to outside
// lower_left_corner is enough to get the camera size, related computation on camera()
class camera {
public:
	camera(point3 lookfrom, point3 lookat,vec3 vup, float vfov, float aspect_ratio, float aperture, float focus_dist) {
		// camera position
		// camera defocus_blur simulation, aperture means ��Ȧ�� focus_dist means ����
		// camera fov and image size 
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		float viewport_height = 2.0 * h;
		float viewport_width = aspect_ratio * viewport_height;
		// float focal_length = 1.0;
		
		w = unit_vector(lookfrom - lookat); // +z
		v = unit_vector(vup); // +y
		u = cross(vup, w); // +x
		
		// ��ģ����ƽ����͸���������ڵ�ƽ�棬ͬʱ��ģ��Ӧ�Ĺ��߷�������͸��ƽ��
		// ���ģ�������˹�Ȧ֮�󣬶�Ӧ��ˮƽ�ʹ�ֱ�����½�λ�÷����仯��focus_distӰ�쳤����
		original = lookfrom; // lookat and vup is vertical with horizontal direction
		horizontal = focus_dist * viewport_width * u; // ��������������ԭ������Ӧ�ĳ���ƽ���СҲ��focal_length = 1.0 ��Ϊ focus_dist
		vertical = focus_dist * viewport_height * v;
		low_left_corner = lookfrom - horizontal / 2 - vertical / 2 -  focus_dist * w; // lower_left_corner position

		lens_radius = aperture / 2;


	}
	// all ray info is start from origin and camera related info
	ray get_ray(float s, float t) const {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() +  v* rd.y();
		return ray(original + offset , low_left_corner + s * horizontal + t * vertical - original - offset);
	}

private:
	point3 original;
	point3 low_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;


};