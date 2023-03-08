#pragma once
#include "rtweekend.h"
#include "vec3.h"
#include "onb.h"


class pdf {
public:
	virtual ~pdf(){}

	virtual float value(const vec3& direction) const = 0;// 计算PDF（direction）

	virtual vec3 generate() const = 0;// 生成一个对应分布的随机数

};


inline vec3 random_cosine_direction() {
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = sqrt(1 - r2);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);

    return vec3(x, y, z);
}


class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

    virtual float value(const vec3& direction) const override {
        float cosine = dot(unit_vector(direction), uvw.w());
        return (cosine < 0) ? 0 : cosine / pi;
    }

    virtual vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

public:
    onb uvw;
};



// light, sampling toward a light
class hittable_pdf : public pdf {
public:
    hittable_pdf(shared_ptr<hittable> p, const point3& origin) : ptr(p), o(origin) {}

    virtual float value(const vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }

    virtual vec3 generate() const override {
        return ptr->random(o);
    }


public:
    point3 o;
    shared_ptr<hittable> ptr;
};


class mixture_pdf : public pdf {
public:
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
        p[0] = p0;
        p[1] = p1;
        if (p[1] == nullptr) p[1] = p[0];
    }

    virtual float value(const vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    virtual vec3 generate() const override {
        if (random_float() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }


public:
    shared_ptr<pdf> p[2];

};





