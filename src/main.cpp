#include <iostream>
#include <vector>
#include "PPMImage.hpp"
#include "global.hpp"
#include "Object.hpp"

Color trace(MRTracer::ray ray, std::vector<MRTracer::Object*>& objects) {
    double t_min = 0, t_max = infinity;
    bool hit = false;
    Color c(166, 166, 166);
    for(MRTracer::Object* o: objects){
        if(MRTracer::hit_record record; o->intersect(ray, record, t_min, t_max)) {
            hit = true, t_max = record.t;
            c = Color((record.normal.x+1)*0.5*255, (record.normal.y+1)*0.5*255, (record.normal.z+1)*0.5*255);
        }
    }
    if(hit) return c;
    double t = (-1-ray.origin().z)/ray.direction().z;
    return c * (1./t);
}

int main() {
    PPMImage image(image_width, image_height);

    MRTracer::vecd horizontal = MRTracer::vecd(r-l, 0, 0, 0);
    MRTracer::vecd vertical = MRTracer::vecd(0, t-b, 0, 0);
    MRTracer::vecd lower_left_corner = MRTracer::vecd(0, 0, -focal_length, 0) - horizontal/2 - vertical/2;

    MRTracer::Sphere sphere(MRTracer::pointd(0,0,-1,1), 0.5);
    MRTracer::Sphere earth(MRTracer::pointd(0,-100.5,-1,1), 100);

    std::vector<MRTracer::Object*> objects;
    objects.push_back(&sphere);
    objects.push_back(&earth);

    for(int i=0;i<image_width;i++){
        for(int j=0;j<image_height;j++){
            double x = (double)i / (image_width-1);
            double y = (double)j / (image_height-1);
            MRTracer::vecd dir = (lower_left_corner + horizontal*x + vertical*y).normalized();
            
            MRTracer::ray ray(origin, dir);
            image.set(i, j, trace(ray, objects));
        }
    }
    image.write_to_file("out.ppm");
    return 0;
}