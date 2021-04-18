#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "algebra.hpp"
#include "ray.hpp"
#include <limits>
#include <random>

constexpr double aspect_ratio = 16./9.;
constexpr int image_height = 900;
constexpr int image_width = image_height * aspect_ratio;
constexpr double t = 1;
constexpr double b = -t;
constexpr double r = t * aspect_ratio;
constexpr double l = -r;
constexpr double focal_length = 1;
const MRTracer::pointd origin(0,0,0,1);

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;

template<typename T>
inline T get_random(T min, T max) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(min, max);
    return dis(gen);
}

#endif