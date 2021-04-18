#ifndef __RAY_H__
#define __RAY_H__
#include "algebra.hpp"

namespace MRTracer{
    class ray {
        pointd o;
        vecd dir;
    public:
        ~ray() = default;
        ray() noexcept {}
        ray(const pointd& origin, const vecd& direction) noexcept
            : o(origin), dir(direction) {
                o.w = 1, dir.w = 0;
            }

        pointd origin() const  { return o; }
        vecd direction() const { return dir; }

        pointd at(double t) const {
            return o + dir * t;
        }
    };
}

#endif