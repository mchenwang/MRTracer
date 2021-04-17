#include "PPMImage.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

PPMImage::PPMImage(int W, int H) noexcept
: width(W), height(H) {
    if(W <= 0 || H <= 0) image = nullptr;
    else image = new Color[W*H];
}

PPMImage::~PPMImage() noexcept {
    if(image != nullptr){
        delete image;
        image = nullptr;
    }
}

PPMImage::PPMImage(const PPMImage& other) noexcept {
    width = other.width;
    height = other.height;
    image = new Color[width*height];
    std::copy(other.image, other.image + width * height, image);
}

void swap(PPMImage& first, PPMImage& second) {
    using std::swap;
    swap(first.width, second.width);
    swap(first.height, second.height);
    swap(first.image, second.image);
}

PPMImage::PPMImage(PPMImage&& other) noexcept
: PPMImage() {
    swap(*this, other);
}

PPMImage& PPMImage::operator=(const PPMImage& other) {
    PPMImage temp(other);
    swap(*this, temp);
    return *this;
}

PPMImage& PPMImage::operator=(PPMImage&& other) {
    swap(*this, other);
    return *this;
}

void PPMImage::set(int x, int y, Color c) {
    if(x < 0 || x >= width || y < 0 || y >= height) return;
    image[x + y*width] = c;
}

Color PPMImage::get(int x, int y) {
    if(x < 0 || x >= width || y < 0 || y >= height) return Color();
    return image[x + y*width];
}

void PPMImage::write_to_file(const char* file) {
    std::ofstream f;
    f.open(file);
    if(!f.is_open()) {
        std::cerr << file << " cannot be open.\n";
        f.close();
        return;
    }
    f << "P3\n" << width << ' ' << height << "\n255\n";

    for(int y = height-1; y >= 0; y--) {
        for(int x = 0; x < width; x++) {
            f << image[x + y*width] << "\n";
        }
    }
    f.close();

    std::cout << file <<" has been saved.\n";
}