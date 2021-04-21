#ifndef __OBJECT_H__
#define __OBJECT_H__
#include "global.hpp"

namespace MRTracer {
    struct hit_record {
        pointd p;
        vecd normal;
        double t;
        bool front_face;

        void set_face_normal(const ray& r) {
            front_face = (r.direction() * normal) <= 0;
            if(!front_face) normal = vecd(0,0,0,0) - normal;
        }
    };

    class Object {
    public:
        Object() = default;
        virtual bool intersect(const ray& r, hit_record&, double, double) const = 0;
    };

    class Sphere: public Object {
        pointd center;
        double radius;
    public:
        Sphere() = default;
        Sphere(pointd c, double r) noexcept : center(c), radius(r) {}
        bool intersect(const ray& r, hit_record& ret, double t_min, double t_max) const override {
            vecd CO = r.origin() - center;
            double a = r.direction().norm2();
            double b = CO * r.direction();
            double c = CO.norm2() - radius*radius;
            double discriminant = b*b - a*c;
            if(discriminant < 0) return false;
            double temp_t = (-b-std::sqrt(discriminant)) / a;
            if(temp_t > t_max || temp_t < t_min) {
                temp_t = (-b+std::sqrt(discriminant)) / a;
                if(temp_t > t_max || temp_t < t_min) return false;
            }
            ret.t = temp_t;
            ret.p = r.at(ret.t);
            ret.normal = (ret.p - center) / radius;
            ret.set_face_normal(r);
            return true;
        }
    };
}

#endif