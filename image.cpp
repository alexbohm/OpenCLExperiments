#include "image.h"

#include <CL/cl.hpp>
#include <iostream>
#include <Magick++.h>
#include <fstream>

Image::Image(cl::Context& context, std::string filename) {
    std::cout << "Opening Image " << filename << std::endl;

    std::ifstream file;
    
    file.open(filename, std::ios::in | std::ios::binary);
    if(!file.is_open()) {
        std::cout << "Error opening image " << filename << std::endl;
        return;
    }
    // Allocate and read the BMP header
    unsigned char BMPheader[54];
    file.read((char*)BMPheader, 54);
    // Extract the BMP header
    width = *(int*)&BMPheader[18]; // get address of width, cast it to an int and then dereference it
    height = *(int*)&BMPheader[22];
    
    // Read the pixels into memory
    pixels = new unsigned char[width * height * 3];
    file.read((char*)pixels, width * height * 3);

    //TODO: Can we leave the pixel format as BGR

    gpu_image = new cl::Image2D(context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGB, CL_UNORM_INT8), width, height, 0, pixels);

    file.close();
}
Image::Image(cl::Context& context, int w, int h) : width(w), height(h) {
    std::cout << "Allocating Image with " << width << " pixels wide and " << height << " pixels tall" << std::endl;
    pixels = new unsigned char[width * height * 3];

    //TODO: Can we leave the pixel format as BGR
    gpu_image = new cl::Image2D(context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGB, CL_UNORM_INT8), width, height, 0, pixels);
}
void Image::enqueueWrite(cl::CommandQueue& queue){
    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = width;
    region[1] = height;
    region[2] = 1;

    queue.enqueueWriteImage(*gpu_image, CL_TRUE, origin, region, 0, 0, pixels);
}
void Image::enqueueRead(cl::CommandQueue& queue){
    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = width;
    region[1] = height;
    region[2] = 1;

    queue.enqueueReadImage(*gpu_image, CL_TRUE, origin, region, 0, 0, pixels);
}
void Image::save(std::string filename){
    std::cout << "Saving to file: " << filename << std::endl;

    // std::ofstream file(filename, std::ios::out | std::ios::binary);
    // if(!file.is_open()){
    //     std::cout << "Error Saving " << filename << std::endl;
    //     return;
    // }
    // // unsigned char BMPheader[54];
    // // BMPheader[0] = 'B';
    // // BMPheader[1] = 'M';
    // int num;
    // file.write("BM", 2);
    // // TODO: this assumes that the image already aligns to 4 bytes
    // num = width * height * 3 + 54;
    // // TODO: this may fail if we don't have a 4 byte int
    // file.write(reinterpret_cast<const char*>(&num),sizeof(num));

    Magick::Image image(width, height, "RGB", Magick::CharPixel, pixels);
    image.write(filename);
}
Image::~Image() {
    delete[] pixels;
    delete gpu_image;
}