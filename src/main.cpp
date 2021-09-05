#include <iostream>
#include <vector>
#include "PPMImage.hpp"
#include "global.hpp"

int main() {

    // create_small_word();
    // create_word();

    // HANDLE* thread_list = new HANDLE[thread_num];
    // std::vector<RenderThreadData> thread_para(thread_num);

    // for(int i = 0, from = 0; i < thread_num; i++) {
    //     int to = (i == thread_num - 1 ? image_width : from + image_width/thread_num);
    //     thread_para[i] = {from, to};
    //     thread_list[i] = CreateThread(NULL, 0, render, &thread_para[i], 0, NULL);
    //     from = to;
    // }
    // WaitForMultipleObjects(thread_num, thread_list, TRUE, INFINITE);

    // delete[] thread_list;
    // image.write_to_file("out.ppm");
    return 0;
}