#include <iostream>
#include <vector>
#include <Windows.h>
#include "PPMImage.hpp"
#include "global.hpp"
#include "Object.hpp"
#include "camera.hpp"

namespace MRTracer {
    vecd trace(ray r, const std::vector<std::unique_ptr<MRTracer::Object>>& objects, int dep = 10) {
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
MRTracer::Camera camera;
std::vector<std::unique_ptr<MRTracer::Object>> objects;

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
            image.set(i, j, Color(r, g, b));
        }
    }
    std::cout<<"Thread "<<GetCurrentThreadId()<<" end\n";
    return 0L;
}

void create_small_word() {
    const MRTracer::pointd eye_pos(3,3,2,1);
    const MRTracer::vecd eye_up_dir(0,1,0,0);
    const MRTracer::pointd center(0,0,-1,1);
    camera = MRTracer::Camera(20, eye_pos, eye_up_dir, center);

    auto material_ground = std::make_shared<MRTracer::Lambertian>(MRTracer::vecd(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<MRTracer::Lambertian>(MRTracer::vecd(0.7, 0.3, 0.3));
    auto material_left   = std::make_shared<MRTracer::Dielectric>(1.5);
    auto material_right  = std::make_shared<MRTracer::Metal>(MRTracer::vecd(1, 1, 1), 0.3);

    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::Sphere(MRTracer::pointd(0,-100.5,-1,1), 100, material_ground)));
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::Sphere(MRTracer::pointd(0,0,-1,1), 0.5, material_center)));
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::Sphere(MRTracer::pointd(-1,0,-1,1), 0.5, material_left)));
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::Sphere(MRTracer::pointd(-1,0,-1,1), -0.4, material_left)));
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::Sphere(MRTracer::pointd(1,0,-1,1), 0.5, material_right)));
}

void create_word() {
    const MRTracer::pointd eye_pos(13,2,3,1);
    const MRTracer::vecd eye_up_dir(0,1,0,0);
    const MRTracer::pointd origin(0,0,-1,1);
    camera = MRTracer::Camera(20, eye_pos, eye_up_dir, origin, 0.05, 10);

    auto ground_material = std::make_shared<MRTracer::Lambertian>(MRTracer::vecd(0.5, 0.5, 0.5));
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::pointd(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = get_random();
            MRTracer::pointd center(a + 0.9*get_random(), 0.2, b + 0.9*get_random(), 1);

            if ((center - MRTracer::pointd(4, 0.2, 0)).norm() > 0.9) {
                std::shared_ptr<MRTracer::Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    MRTracer::vecd albedo(get_random(), get_random(), get_random());
                    sphere_material = std::make_shared<MRTracer::Lambertian>(albedo);
                    objects.push_back(std::make_unique<MRTracer::Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    MRTracer::vecd albedo(get_random(.5, 1.), get_random(.5, 1.), get_random(.5, 1.));
                    double fuzz = get_random(0, 0.5);
                    sphere_material = std::make_shared<MRTracer::Metal>(albedo, fuzz);
                    objects.push_back(std::make_unique<MRTracer::Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<MRTracer::Dielectric>(1.5);
                    objects.push_back(std::make_unique<MRTracer::Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<MRTracer::Dielectric>(1.5);
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::pointd(0, 1, 0, 1), 1, material1));

    auto material2 = std::make_shared<MRTracer::Lambertian>(MRTracer::vecd(0.4, 0.2, 0.1));
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::pointd(-4, 1, 0, 1), 1, material2));

    auto material3 = std::make_shared<MRTracer::Metal>(MRTracer::vecd(0.7, 0.6, 0.5), 0.0);
    objects.push_back(std::make_unique<MRTracer::Sphere>(MRTracer::pointd(4, 1, 0, 1), 1, material3));
}

int main() {

    // create_small_word();
    create_word();

    HANDLE* thread_list = new HANDLE[thread_num];
    std::vector<RenderThreadData> thread_para(thread_num);

    for(int i = 0, from = 0; i < thread_num; i++) {
        int to = (i == thread_num - 1 ? image_width : from + image_width/thread_num);
        thread_para[i] = {from, to};
        thread_list[i] = CreateThread(NULL, 0, render, &thread_para[i], 0, NULL);
        from = to;
    }
    WaitForMultipleObjects(thread_num, thread_list, TRUE, INFINITE);

    delete[] thread_list;
    image.write_to_file("out.ppm");
    return 0;
}