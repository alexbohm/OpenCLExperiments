#include "image.h"

#include <CL/cl.hpp>
#include <iostream>
#include <Magick++.h>
#include <fstream>

Image::Image(cl::Context& context, std::string filename) {
    std::cout << "Opening Image " << filename << std::endl;

    Magick::Image image(filename);

    width = image.columns();
    height = image.rows();
    pixels = new unsigned char[width * height * 3];

    image.magick("RGB");
    image.depth(8);
    image.getPixels(0, 0, width, height);
    image.writePixels(Magick::RGBQuantum, pixels);

    int error;
    gpu_image = new cl::Image2D(context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGB, CL_UNORM_INT8), width, height, 0, NULL, &error);
    
    std::cout << "    Image2D Status: " << error << std::endl;
}
Image::Image(cl::Context& context, int w, int h) : width(w), height(h) {
    std::cout << "Allocating Image with " << width << " pixels wide and " << height << " pixels tall" << std::endl;
    pixels = new unsigned char[width * height * 3];

    int error;
    gpu_image = new cl::Image2D(context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGB, CL_UNORM_INT8), width, height, 0, NULL, &error);

    std::cout << "    Image2D Status: " << error << std::endl;
}
void Image::enqueueWrite(cl::CommandQueue& queue){
    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = width / 2;
    region[1] = height / 2;
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