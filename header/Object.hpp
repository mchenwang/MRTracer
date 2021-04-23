#ifndef __OBJECT_H__
#define __OBJECT_H__
#include "global.hpp"
#include <memory>

namespace MRTracer {
    class Material;
    struct hit_record {
        pointd p;
        vecd normal;
        double t;
        bool front_face;
        std::shared_ptr<Material> material;

        void set_face_normal(const ray& r) {
            front_face = (r.direction() * normal) <= 0;
            if(!front_face) normal = vecd(0,0,0,0) - normal;
        }
    };
    
    class Material {
    public:
        virtual bool scatter(const ray& r, const hit_record& record, vecd& attenuation, ray& scattered) const = 0;
    };

    class Object {
    protected:
        std::shared_ptr<Material> material;
    public:
        Object() noexcept {}
        Object(std::shared_ptr<Material> m) noexcept : material(m) {}
        virtual bool intersect(const ray& r, hit_record&, double, double) const = 0;
    };

    class Sphere: public Object {
        pointd center;
        double radius;
    public:
        Sphere() = default;
        Sphere(pointd c, double r, std::shared_ptr<Material> m=nullptr) noexcept 
        : Object(m), center(c), radius(r) {}
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
            ret.material = material;
            return true;
        }
    };

    vecd get_rand_vec_in_sphere() {
        while(true) {
            vecd ret(get_random(-1, 1), get_random(-1, 1), get_random(-1, 1), 0);
            if(ret.norm2() >= 1) continue;
            return ret;
        }
    }

    vecd get_rand_unit_vec_in_sphere() { return get_rand_vec_in_sphere().normalized(); }

    vecd get_rand_unit_vec_in_hemisphere(const vecd& nm) {
        vecd ret = get_rand_unit_vec_in_sphere();
        return (ret * nm > 0 ? ret : vecd(0,0,0) - ret);
    }

    class Lambertian: public Material {
        vecd albedo;
    public:
        Lambertian(const vecd& a) noexcept : albedo(a) {}
        bool scatter(const ray&, const hit_record& record, vecd& attenuation, ray& scattered) const override {
            vecd scatter_dir = (record.normal + get_rand_unit_vec_in_sphere()).normalized();
            if(scatter_dir.is_zero_vec()) scatter_dir = record.normal;
            scattered = ray(record.p, scatter_dir);
            attenuation = albedo;
            return true;
        }
    };

    class Metal: public Material {
        vecd albedo;
        double fuzz;
    public:
        Metal(const vecd& a, double f = 0.) noexcept : albedo(a), fuzz(f<1.?f:1.) {}
        bool scatter(const ray& r, const hit_record& record, vecd& attenuation, ray& scattered) const override {
            vecd reflect_dir = r.direction() - record.normal * (r.direction() * record.normal * 2.);
            scattered = ray(record.p, (reflect_dir + get_rand_unit_vec_in_sphere()*fuzz).normalized());
            attenuation = albedo;
            return scattered.direction() * record.normal > 0;
        }
    };

    class Dielectric: public Material {
        vecd refract_attenuation;
        double refract_index;
        static double reflectance(double cos_theta, double eta) {
            double r0 = (1.-eta) / (1.+eta);
            r0 = r0 * r0;
            // std::cout<< r0 + (1. - r0) * std::pow(1 - cos_theta, 5)<<" ";
            return r0 + (1. - r0) * std::pow(1 - cos_theta, 5);
        }
    public:
        Dielectric(double r_i = 1.) noexcept : refract_attenuation(vecd(1,1,1)), refract_index(r_i) {}
        Dielectric(vecd r_a, double r_i = 1.) noexcept : refract_attenuation(r_a), refract_index(r_i) {}
        bool scatter(const ray& r, const hit_record& record, vecd& attenuation, ray& scattered) const override {
            double eta = record.front_face ? (1./refract_index) : refract_index;
            double cos_theta = - (r.direction() * record.normal);
            if(cos_theta > 1.) cos_theta = 1.;
            double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
            if(eta * sin_theta > 1. || reflectance(cos_theta, eta) >= 1.) {
                vecd reflect_dir = r.direction() - record.normal * (r.direction() * record.normal * 2.);
                scattered = ray(record.p, reflect_dir.normalized());
            } else {
                vecd r_vertical = (r.direction() - record.normal*(r.direction()*record.normal)) * eta;
                vecd r_parallel = record.normal * (-std::sqrt(std::abs(1.0 - r_vertical.norm2())));
                scattered = ray(record.p, (r_vertical + r_parallel).normalized());
            }
            attenuation = refract_attenuation;
            return true;
        }
    };
}

#endif