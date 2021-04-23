#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "algebra.hpp"
#include "ray.hpp"
#include <limits>
#include <random>

constexpr double aspect_ratio = 16./9.;
constexpr int image_height = 900;
constexpr int image_width = image_height * aspect_ratio;
// constexpr double t = 1;
// constexpr double b = -t;
// constexpr double r = t * aspect_ratio;
// constexpr double l = -r;
// constexpr double focal_length = 1;
// const MRTracer::pointd origin(0,0,0,1);
constexpr int samples_per_pixel = 100;

constexpr int thread_num = 12;

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;

inline double get_random(double min=0., double max=1.) {
    // 贼慢
    // static std::uniform_real_distribution<double> distribution(min, max);
    // static std::mt19937 generator;
    // return distribution(generator);

    return min + (rand() / (RAND_MAX + 1.0)) * (max - min);
}

#endif