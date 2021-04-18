#ifndef __OBJECT_H__
#define __OBJECT_H__
#include "global.hpp"

namespace MRTracer {
    struct hit_record {
        pointd p;
        vecd normal;
        double t;
    };

    class Object {
    protected:
    public:
        virtual bool intersect(const ray& r, hit_record&, double, double) const = 0;
    };

    class Sphere: public Object {
        pointd center;
        double radius;
    public:
        Sphere() {}
        Sphere(const pointd& c, double r): center(c), radius(r) {}
        bool intersect(const ray& r, hit_record& ret, double t_min, double t_max) const override {
            vecd CO = r.origin() - center;
            double a = r.direction().norm2();
            double b = CO * r.direction();
            double c = CO.norm2() - radius*radius;
            double discriminant = b*b - a*c;
            if(discriminant < 0) return false;
            ret.t = (-b-std::sqrt(discriminant)) / a;
            if(ret.t > t_max || ret.t < t_min) {
                ret.t = (-b+std::sqrt(discriminant)) / a;
                if(ret.t > t_max || ret.t < t_min) return false;
            }
            ret.p = r.at(ret.t);
            ret.normal = (ret.p - center) / radius;
            return true;
        }
    };
}

#endif