#include <iostream>
#include <vector>
#include <Windows.h>
#include "PPMImage.hpp"
#include "global.hpp"
#include "Object.hpp"
#include "camera.hpp"

namespace MRTracer {

    vecd trace(ray r, const std::vector<const Object*>& objects, int dep = 10) {
        if(dep <= 0) return vecd(0,0,0);
        double t_min = 0.00001, t_max = infinity;
        bool hit = false;
        vecd c;
        hit_record record;
        for(const auto& o: objects){
            if(o->intersect(r, record, t_min, t_max)) {
                hit = true, t_max = record.t;
                c = vecd((record.normal.x+1)*0.5, (record.normal.y+1)*0.5, (record.normal.z+1)*0.5);
            }
        }
        if(hit) {
            ray scattered;
            vecd attenuation;
            if(record.material->scatter(r, record, attenuation, scattered))
                return outer_product(attenuation, trace(scattered, objects, dep-1));
            return vecd(0,0,0);
            // vecd dir = (record.normal + get_rand_vec_in_hemisphere(record.normal)).normalized();
            // vecd dir = get_rand_unit_vec_in_hemisphere(record.normal);
            // return trace(MRTracer::ray(record.p, dir), objects, dep-1) * 0.5;
        }
        double t = 0.5*(r.direction().y + 1.0);
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
                auto temp = trace(camera.get_ray(x, y), objects, 50);
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
    std::cout<<"Thread "<<GetCurrentThreadId()<<" end\n";
    return 0L;
}

int main() {
    auto material_ground = std::make_shared<MRTracer::Lambertian>(MRTracer::vecd(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<MRTracer::Lambertian>(MRTracer::vecd(0.7, 0.3, 0.3));
    auto material_left   = std::make_shared<MRTracer::Metal>(MRTracer::vecd(1, 1, 1), 0.3);
    // auto material_right  = std::make_shared<MRTracer::Metal>(MRTracer::vecd(1, 1, 1), 0);
    auto material_right  = std::make_shared<MRTracer::Dielectric>(1.5);

    // const MRTracer::Sphere sphere(MRTracer::pointd(0,0,-1,1), 0.5);
    const MRTracer::Sphere earth(MRTracer::pointd(0,-100.5,-1,1), 100, material_ground);
    const MRTracer::Sphere sphere_c(MRTracer::pointd(0,0,-1,1), 0.5, material_center);
    const MRTracer::Sphere sphere_l(MRTracer::pointd(-1,0,-1,1), 0.5, material_left);
    const MRTracer::Sphere sphere_r(MRTracer::pointd(1,0,-1,1), 0.5, material_right);
    const MRTracer::Sphere sphere_rr(MRTracer::pointd(1,0,-1,1), -0.4, material_right);
    // const MRTracer::Sphere sphere1(MRTracer::pointd(0,0,-1,1), 0.5, material_left);
    // const MRTracer::Sphere sphere2(MRTracer::pointd(-1,0,-1,1), 0.5, material_center);
    // const MRTracer::Sphere sphere3(MRTracer::pointd(1,0,-1,1), 0.5, material_center);

    objects.push_back(&earth);
    objects.push_back(&sphere_c);
    objects.push_back(&sphere_l);
    objects.push_back(&sphere_r);
    objects.push_back(&sphere_rr);

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