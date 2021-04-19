#ifndef __PPMIMAGE_H__
#define __PPMIMAGE_H__
#include <iostream>
#include <fstream>
typedef unsigned char uint8_t;

struct Color {
    uint8_t r, g, b;
    Color(int _r=0, int _g=0, int _b=0)
    : r((_r > 255 ? 255 : (_r < 0 ? 0 : _r))),
      g((_g > 255 ? 255 : (_g < 0 ? 0 : _g))),
      b((_b > 255 ? 255 : (_b < 0 ? 0 : _b))) {}

    Color operator*(double c) { return Color(r*c, g*c, b*c); }
    Color& operator*=(double c) { r*=c, g*=c, b*=c; return *this; }

    template<typename Tstream>
    friend Tstream& operator<<(Tstream& out, const Color& c) {
        out << (int)c.r << " " << (int)c.g << " " << (int)c.b;
        return out;
    }
};

class PPMImage {
    int width;
    int height;
    Color* image;
    friend void swap(PPMImage&, PPMImage&);
public:
    PPMImage(int W = 0, int H = 0) noexcept;
    ~PPMImage() noexcept;
    PPMImage(const PPMImage& other) noexcept;
    PPMImage(PPMImage&& other) noexcept;
    PPMImage& operator=(const PPMImage& other);
    PPMImage& operator=(PPMImage&& other);

    void set(int x, int y, Color c);
    Color get(int x, int y);
    int get_width() { return width; }
    int get_height() { return height; }

    void write_to_file(const char* file);
};

#endif