#include <iostream>
#include <vector>
#include <Windows.h>
#include "PPMImage.hpp"
#include "global.hpp"
#include "Object.hpp"
#include "camera.hpp"

static inline MRTracer::vecd get_rand_vec() {
    while(true){
        MRTracer::vecd ret(get_random(-1, 1), get_random(-1, 1), get_random(-1, 1), 0);;
        if(ret.norm2() >= 1) continue;
        return ret;
    }
}

namespace MRTracer {
    Color trace(ray ray, std::vector<Object*>& objects, int dep = 10) {
        if(dep <= 0) return Color(0,0,0);
        double t_min = 0, t_max = infinity;
        bool hit = false;
        Color c;
        hit_record record;
        for(Object* o: objects){
            if(o->intersect(ray, record, t_min, t_max)) {
                hit = true, t_max = record.t;
                c = Color((record.normal.x+1)*0.5*255, (record.normal.y+1)*0.5*255, (record.normal.z+1)*0.5*255);
            }
        }
        if(hit) {
            vecd s = (record.normal + get_rand_vec()).normalized();
            return trace(MRTracer::ray(record.p, s), objects, dep-1) * 0.8;
        }
        double t = 0.5*(ray.direction().y + 1.0);
        return Color((1-0.5*t)*255, (1-0.3*t)*255, 255);
    }
}

PPMImage image(image_width, image_height);
MRTracer::Camera camera;
std::vector<MRTracer::Object*> objects;

struct RenderThreadData { int i_from, i_to; };
DWORD WINAPI render(LPVOID range_) {
    // srand(GetCurrentThreadId());
    RenderThreadData* range = (RenderThreadData*)range_;
    for(int i = range->i_from; i < range->i_to; i++){
        for(int j = 0; j < image_height; j++){
            int r = 0, g = 0, b = 0;
            for(int k = 0; k < samples_per_pixel; k++){
                double x = (i + get_random(0., 1.)) / (image_width - 1);
                double y = (j + get_random(0., 1.)) / (image_height- 1);
                Color temp = trace(camera.get_ray(x, y), objects, 10);
                r += temp.r, g += temp.g, b += temp.b;
            }
            image.set(i, j, Color(r/samples_per_pixel, g/samples_per_pixel, b/samples_per_pixel));
        }
    }
    std::cout<<"Thread "<<GetCurrentThreadId()<<" end\n";
    return 0L;
}

int main() {
    MRTracer::Sphere sphere(MRTracer::pointd(0,0,-1,1), 0.5);
    MRTracer::Sphere earth(MRTracer::pointd(0,-100.5,-1,1), 100);

    objects.push_back(&sphere);
    objects.push_back(&earth);

    HANDLE* thread_list = new HANDLE[thread_num];
    std::vector<RenderThreadData> thread_para(thread_num);

    for(int i = 0, from = 0; i < thread_num; i++) {
        int to = (i == thread_num - 1 ? image_width : from + image_width/thread_num);
        thread_para[i] = {from, to};
        thread_list[i] = CreateThread(NULL, 0, render, &thread_para[i], 0, NULL);
        from = to;
    }
    WaitForMultipleObjects(thread_num, thread_list, TRUE, INFINITE);
    // for(int i = 0; i < image_width; i++){
    //     for(int j = 0; j < image_height; j++){
    //         int r = 0, g = 0, b = 0;
    //         for(int k = 0; k < samples_per_pixel; k++){
    //             double x = (i + get_random(0., 1.)) / (image_width - 1);
    //             double y = (j + get_random(0., 1.)) / (image_height- 1);
    //             Color temp = trace(camera.get_ray(x, y), objects);
    //             r += temp.r, g += temp.g, b += temp.b;
    //         }
    //         image.set(i, j, Color(r/samples_per_pixel, g/samples_per_pixel, b/samples_per_pixel));
    //     }
    // }
    image.write_to_file("out.ppm");
    return 0;
}