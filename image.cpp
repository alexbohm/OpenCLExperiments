#include "image.h"

#include <CL/cl.hpp>
#include <iostream>
#include <Magick++.h>
#include <fstream>

Image::Image(cl::Context& context, std::string filename) {
    std::cout << "Opening Image " << filename << std::endl;

    // std::ifstream file;
    
    // file.open(filename, std::ios::in | std::ios::binary);
    // if(!file.is_open()) {
    //     std::cout << "Error opening image " << filename << std::endl;
    //     return;
    // }
    // // Allocate and read the BMP header
    // unsigned char BMPheader[54];
    // file.read((char*)BMPheader, 54);
    // // Extract the BMP header
    // width = *(int*)&BMPheader[18]; // get address of width, cast it to an int and then dereference it
    // height = *(int*)&BMPheader[22];
    
    // // Read the pixels into memory
    // pixels = new unsigned char[width * height * 3];
    // file.read((char*)pixels, width * height * 3);

    Magick::Image image(filename);

    width = image.columns();
    height = image.rows();
    pixels = new unsigned char[width * height * 3];

    image.magick("RGB");
    image.depth(8);
    image.getPixels(0, 0, width, height);
    image.writePixels(Magick::RGBQuantum, pixels);


    // gpu_image = new cl::Image2D(context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGB, CL_UNORM_INT8), width, height, 0, pixels);

    // file.close();
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
    
    Magick::Image image;
    image.size("2592x1944");
    image.magick("BMP");
    image.depth(8);
    image.setPixels(0, 0, width, height);
    image.readPixels(Magick::RGBQuantum, pixels);

    image.write(filename);

}
Image::~Image() {
    delete[] pixels;
    delete gpu_image;
}