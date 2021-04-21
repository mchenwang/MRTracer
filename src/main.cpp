#include <iostream>
#include <vector>
#include <Windows.h>
#include "PPMImage.hpp"
#include "global.hpp"
#include "Object.hpp"
#include "camera.hpp"

static MRTracer::vecd get_rand_vec() {
    while(true){
        MRTracer::vecd ret(get_random(-1, 1), get_random(-1, 1), get_random(-1, 1), 0);
        if(ret.norm2() >= 1) continue;
        return ret.normalized();
        // return ret;
    }
}

namespace MRTracer {
    vecd trace(ray ray, const std::vector<const Object*>& objects, int dep = 10) {
        if(dep <= 0) return vecd(0,0,0);
        double t_min = 0.000001, t_max = infinity;
        bool hit = false;
        vecd c;
        hit_record record;
        for(const auto& o: objects){
            if(o->intersect(ray, record, t_min, t_max)) {
                hit = true, t_max = record.t;
                c = vecd((record.normal.x+1)*0.5, (record.normal.y+1)*0.5, (record.normal.z+1)*0.5);
            }
        }
        if(hit) {
            vecd dir = (record.normal + get_rand_vec()).normalized();
            return trace(MRTracer::ray(record.p, dir), objects, dep-1) * 0.5;
        }
        double t = 0.5*(ray.direction().y + 1.0);
        return vecd((1-0.5*t), (1-0.3*t), 1.);
    }
}

PPMImage image(image_width, image_height);
const MRTracer::Camera camera;
std::vector<const MRTracer::Object*> objects;
// HANDLE hMutex;

struct RenderThreadData { int i_from, i_to; };
DWORD WINAPI render(LPVOID range_) {
    RenderThreadData* range = (RenderThreadData*)range_;
    srand(range->i_from);
    for(int i = range->i_from; i < range->i_to; i++){
        for(int j = 0; j < image_height; j++){
            double r = 0, g = 0, b = 0;
            for(int k = 0; k < samples_per_pixel; k++){
                double x = (i + get_random(0., 1.)) / (image_width - 1);
                double y = (j + get_random(0., 1.)) / (image_height- 1);
                auto temp = trace(camera.get_ray(x, y), objects, 10);
                r += temp[0], g += temp[1], b += temp[2];
            }
            r = std::sqrt(r/samples_per_pixel)*255;
            g = std::sqrt(g/samples_per_pixel)*255;
            b = std::sqrt(b/samples_per_pixel)*255;
            // r = (r/samples_per_pixel)*255;
            // g = (g/samples_per_pixel)*255;
            // b = (b/samples_per_pixel)*255;
            // WaitForSingleObject(hMutex, INFINITE);
            image.set(i, j, Color(r, g, b));
            // ReleaseMutex(hMutex);
        }
    }
    // std::cout<<range->i_from<<" "<<range->i_to<<"\n";
    std::cout<<"Thread "<<GetCurrentThreadId()<<" end\n";
    return 0L;
}

int main() {

    const MRTracer::Sphere sphere(MRTracer::pointd(0,0,-1,1), 0.5);
    const MRTracer::Sphere earth(MRTracer::pointd(0,-100.5,-1,1), 100);

    objects.push_back(&sphere);
    objects.push_back(&earth);

    HANDLE* thread_list = new HANDLE[thread_num];
    std::vector<RenderThreadData> thread_para(thread_num);
    // hMutex = CreateMutex(NULL, false, NULL);

    for(int i = 0, from = 0; i < thread_num; i++) {
        int to = (i == thread_num - 1 ? image_width : from + image_width/thread_num);
        thread_para[i] = {from, to};
        thread_list[i] = CreateThread(NULL, 0, render, &thread_para[i], 0, NULL);
        from = to;
    }
    WaitForMultipleObjects(thread_num, thread_list, TRUE, INFINITE);
    delete[] thread_list;

    // #pragma omp parallel for
    // for(int i = 0; i < image_width; i++){
    //     for(int j = 0; j < image_height; j++){
    //         double r = 0, g = 0, b = 0;
    //         for(int k = 0; k < samples_per_pixel; k++){
    //             double x = (i + get_random(0., 1.)) / (image_width - 1);
    //             double y = (j + get_random(0., 1.)) / (image_height- 1);
    //             auto temp = trace(camera.get_ray(x, y), objects, 10);
    //             r += temp[0], g += temp[1], b += temp[2];
    //         }
    //         r = std::sqrt(r/samples_per_pixel)*255;
    //         g = std::sqrt(g/samples_per_pixel)*255;
    //         b = std::sqrt(b/samples_per_pixel)*255;
    //         image.set(i, j, Color(r, g, b));
    //     }
    // }
    image.write_to_file("out.ppm");
    return 0;
}