#include <iostream>
#include "PPMImage.hpp"
#include "algebra.hpp"

constexpr double aspect_ratio = 16./9.;
constexpr int image_height = 900;
constexpr int image_width = image_height * aspect_ratio;
constexpr double t = 450;
constexpr double b = -t;
constexpr double r = t * aspect_ratio;
constexpr double l = -r;
const MRTracer::pointd origin(0,0,0,1);

Color get(int sx, int sy) {
    
    double x = (double)sx/(image_width-1)*(r-l) + l;
    double y = (double)sy/(image_height-1)*(t-b) + b;
    MRTracer::pointd p(x/r,y/t,-1,1);
    MRTracer::vecd ray = p - origin;
    // std::cout<<sx<<" "<<sy<<" \n";
    return Color(167, 168, 189) * (1/ray.norm2());
}

int main() {
    PPMImage image(image_width, image_height);
    for(int i=0;i<image_width;i++){
        for(int j=0;j<image_height;j++){
            image.set(i, j, get(i, j));
        }
    }
    image.write_to_file("out.ppm");
    return 0;
}