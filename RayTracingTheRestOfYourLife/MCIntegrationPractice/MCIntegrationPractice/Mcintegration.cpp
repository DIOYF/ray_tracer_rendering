#include "rtweekend.h"
#include "vec3.h"
#include <iomanip>
#include <stdlib.h>
#include <iostream>


class sampleInUnitSphere {
public:
	inline float pdf(const vec3& p) {
		return 1.0f / (4 * pi);
	}

	void run() {
		int N = 1000000;
		float sum = 0.0f;

		for (int i = 0; i < N; i++) {
			vec3 d = random_unit_vector();
			
			
			float consine_squared = d.z() * d.z();
			sum += consine_squared / pdf(d);
		}

		std::cout << std::fixed << std::setprecision(12);
		std::cout << "Integration cos^2(theta) = " << sum / N << '\n';
		std::cout << "compute right answer = " << 4.0f / 3.0f * pi << '\n';
	}
};


class sampleUnit1DSquaredX {
public:
	inline float pdf(const float& p) {
		return 1.0f / (2.0f);
	}

	void run_mc() {
		int N = 1000000;
		float sum = 0.0f;

		for (int i = 0; i < N; i++) {
			float d = 2 * random_float();
			float d_squared = d * d;
			sum += d_squared / pdf(d);
		}

		std::cout << std::fixed << std::setprecision(12);
		std::cout << "Integration x^2 in (0,2) = " << sum / N << '\n';
		std::cout << "compute right answer = " << 8.0f / 3.0f << '\n';
	}

	void run_unit() {
		int N = 1000000;
		float sum = 0.0f;

		for (int i = 0; i < N; i++) {
			float d = 2 * random_float();


			float d_squared = d * d;
			sum +=  d_squared;
		}

		std::cout << std::fixed << std::setprecision(12);
		std::cout << "Integration x^2 in (0,2) = " << 2 * sum / N << '\n';
		std::cout << "compute right answer = " << 8.0f / 3.0f << '\n';
	}
};

inline float random_double()
{
	return rand() / (RAND_MAX + 1.0);
}


inline float random_double(double min, double max)
{
	return min + random_double() * (max - min);
}


// 使用原书蒙特卡洛方法采样错误，因为如果采样到0，无法相除就会出现nan(ind),所以我们调整采样区间
class sampleLinearPDF1DSquaredX {
public:
	/*
	inline double pdf(double x) {
		return 0.5 * x;
	}

	void run_mc() {
		int N = 1000000;
		double sum = 0.0;
		for (int i = 0; i < N; i++) {
			double x = sqrt(random_double(0, 4));
			sum += x * x / pdf(x);
		}
		std::cout << std::fixed << std::setprecision(12);
		std::cout << "I = " << sum / N << '\n';
	}

	*/
	
	inline float pdf(const float& p) {
		return p * 0.5;
	}

	void run_mc() {
		int N = 1000000;
		float sum = 0.0f;

		for (int i = 0; i < N; i++) {
			float d_squared = random_float(1e-7, 4.0); // 考虑浮点数精度从1e-7开始采样
			sum += d_squared / pdf(sqrtf(d_squared));
		}

		//std::cout << std::fixed << std::setprecision(12);
		std::cout << "Integration x^2 in (0,2) = " << sum / N << '\n';
		std::cout << "compute right answer = " << 8.0f / 3.0f << '\n';
	}
	

};




int main() {
	switch (2) {
	case 0:
		sampleInUnitSphere testsample;
		testsample.run();
		break;
	case 1:
		sampleUnit1DSquaredX test1sample;
		test1sample.run_mc();
		test1sample.run_unit();
		break;
	case 2:
		sampleLinearPDF1DSquaredX test2sample;
		test2sample.run_mc();
		break;
	default:
		break;
	}
	return 0;
}




