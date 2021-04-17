#include <iostream>
#include "PPMImage.hpp"
#include "algebra.hpp"
#include "ray.hpp"

constexpr double aspect_ratio = 16./9.;
constexpr int image_height = 900;
constexpr int image_width = image_height * aspect_ratio;
constexpr double t = 1;
constexpr double b = -t;
constexpr double r = t * aspect_ratio;
constexpr double l = -r;
constexpr double focal_length = 1;
const MRTracer::pointd origin(0,0,0,1);

bool hit_sphere(const MRTracer::pointd& center, double radius, const MRTracer::ray& r) {
    MRTracer::vecd oc = r.origin() - center;
    auto a = r.direction() * r.direction();
    auto b = oc * r.direction() * 2;
    auto c = oc.norm2() - radius*radius;
    auto discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

Color trace(MRTracer::ray ray) {
    Color c(166, 166, 166);
    if(hit_sphere(MRTracer::pointd(0,0,-1), 0.5, ray)) c = Color(0, 127, 0);
    double t = (-1-ray.origin().z)/ray.direction().z;
    return c * (1./t);
}

int main() {
    PPMImage image(image_width, image_height);

    MRTracer::vecd horizontal = MRTracer::vecd(r-l, 0, 0, 0);
    MRTracer::vecd vertical = MRTracer::vecd(0, t-b, 0, 0);
    MRTracer::vecd lower_left_corner = MRTracer::vecd(0, 0, -focal_length, 0) - horizontal/2 - vertical/2;

    for(int i=0;i<image_width;i++){
        for(int j=0;j<image_height;j++){
            double x = (double)i / (image_width-1);
            double y = (double)j / (image_height-1);
            MRTracer::vecd dir = (lower_left_corner + horizontal*x + vertical*y).normalized();
            
            // std::cout<<dir<<"\n";
            MRTracer::ray ray(origin, dir);
            image.set(i, j, trace(ray));
        }
    }
    image.write_to_file("out.ppm");
    return 0;
}