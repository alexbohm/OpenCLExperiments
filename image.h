#pragma once

#include <CL/cl.hpp>
#include <string>

class Image
{
 private:
    unsigned char* pixels;
    cl::Image2D* gpu_image;
    int width;
    int height;
 public:
    Image(cl::Context& context, std::string filename);
    Image(cl::Context& context, int w, int h);
    /**
     * @brief Write the image to the OpenCL device
     */
    void enqueueWrite(cl::CommandQueue& queue);
    /**
     * @brief Read the image from the OpenCL device
     */
    void enqueueRead(cl::CommandQueue& queue);
    /**
     * @brief Save the image to the given filename 
     */
    void save(std::string filename);
    int getWidth() { return width; }
    int getHeight(){ return height; }
    ~Image();
};