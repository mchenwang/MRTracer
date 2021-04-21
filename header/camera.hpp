#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "global.hpp"

namespace MRTracer {
    class Camera {
        vecd horizontal;
        vecd vertical;
        vecd lower_left_corner;
        pointd origin;
    public:
        Camera() noexcept
        : horizontal(vecd(2*aspect_ratio, 0, 0, 0)),
          vertical(vecd(0, 2, 0, 0)),
          lower_left_corner(vecd(0, 0, -1, 0) - horizontal/2 - vertical/2),
          origin(pointd(0,0,0,1)) {}
        
        Camera(double l_, double r_, double b_, double t_, double focal_, pointd ori_) noexcept {
            horizontal = vecd(r_-l_, 0, 0, 0);
            vertical = vecd(0, t_-b_, 0, 0);
            lower_left_corner = vecd(0, 0, -focal_, 0) - horizontal/2 - vertical/2;
            origin = ori_;
        }

        ray get_ray(double x, double y) const {
            return ray(origin, (lower_left_corner + horizontal*x + vertical*y).normalized());
        }
    };
}

#endif