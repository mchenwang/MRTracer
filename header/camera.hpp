#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "global.hpp"

namespace MRTracer {
    class Camera {
        pointd origin;
        mat4d view_matrix;
        double lens_radius;
        vecd u, v;
    public:

        Camera() noexcept {
            double horizontal = 2*aspect_ratio;
            double vertical = 2;
            set_view_matrix(horizontal, vertical, 1);
            origin = pointd(0,0,0,1);
        }
        
        Camera(double l_, double r_, double b_, double t_, double focal_, pointd ori_) noexcept {
            double horizontal = r_ - l_;
            double vertical = t_ - b_;
            set_view_matrix(horizontal, vertical, focal_);
            origin = ori_;
        }

        Camera(double eye_fov, const pointd eye_pos, const vecd eye_up_dir, const pointd center, double lens_r = 1., double dist_to_focus = 10.) noexcept
        : origin(eye_pos), lens_radius(lens_r) {
            double t = std::tan(eye_fov * PI / 180 * 0.5);
            double r = t * aspect_ratio;
            vecd z = (eye_pos - center).normalized();
            u = cross(eye_up_dir, z).normalized();
            v = cross(z, u).normalized();
            double W = 2 * r, H = 2 * t;
            vecd x = u*W*dist_to_focus;
            vecd y = v*H*dist_to_focus;
            z *= dist_to_focus;
            view_matrix[0][0] = x.x, view_matrix[0][1] = y.x, view_matrix[0][2] = -z.x, view_matrix[0][3] = -0.5*x.x-0.5*y.x;
            view_matrix[1][0] = x.y, view_matrix[1][1] = y.y, view_matrix[1][2] = -z.y, view_matrix[1][3] = -0.5*x.y-0.5*y.y;
            view_matrix[2][0] = x.z, view_matrix[2][1] = y.z, view_matrix[2][2] = -z.z, view_matrix[2][3] = -0.5*x.z-0.5*y.z;
            view_matrix[3] = vecd(0,0,0,0);
        }

        void set_view_matrix(double horizontal, double vertical, double focal) {
            view_matrix[0][0] = horizontal, view_matrix[0][3] = -0.5*horizontal;
            view_matrix[1][1] = vertical, view_matrix[1][3] = -0.5*vertical;
            view_matrix[2][2] = -focal;
            view_matrix[3][3] = 0;
        }

        ray get_ray(double x, double y) const {
            vecd rd = random_in_unit_disk() * lens_radius;
            vecd offset = u * rd.x + v * rd.y;
            return ray(origin + offset, (view_matrix * vecd(x, y, 1, 1) - offset).normalized());
        }
    };
}

#endif