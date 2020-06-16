
#include <CL/cl.hpp>
#include <vector>
#include <string>

#include "image.h"

#include <iostream>
using std::cout;
using std::endl;

int main(int argc, char const *argv[])
{
    std::vector<cl::Platform> all_cl_platforms;
    cl::Platform::get(&all_cl_platforms);
    cout << "Found " << all_cl_platforms.size() << " Platforms:" << endl;
    for(cl::Platform& platform : all_cl_platforms) {
        cout << "    " << platform.getInfo<CL_PLATFORM_NAME>() << endl;
    }
    if(all_cl_platforms.size() == 0){
        cout << "Failed to find any OpenCL platforms" << endl;
        return 1;
    }

    std::vector<cl::Device> all_cl_devices;
    all_cl_platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &all_cl_devices);
    cout << "Found " << all_cl_devices.size() << " Devices:" << endl;
    for(cl::Device& device : all_cl_devices){
        cout << "    " << device.getInfo<CL_DEVICE_NAME>() << endl;
        cout << "        " << device.getInfo<CL_DEVICE_EXTENSIONS>() << endl;
    }

    cl::Device default_device = all_cl_devices[0];
    cout << "I will use: " << default_device.getInfo<CL_DEVICE_NAME>() << endl;
    cout << "This device has: " << default_device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << " memory." << endl;
    cout << "This device has these extensions: " << default_device.getInfo<CL_DEVICE_EXTENSIONS>() << endl;
    cl::Context context({default_device});

    cl::Program::Sources sources;

    // std::string kernel_code =
    //         "   void kernel simple_add(global const int* A, global const int* B, global int* C){       "
    //         "       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];                 "
    //         "   }                                                                               ";

std::string kernel_code =
"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;"
"void kernel image_subtract("
"    read_only image2d_t top,"
"    read_only image2d_t bottom,"
"    write_only image2d_t output) {"
"    const int2 pos = {get_global_id(0), get_global_id(1)};"
"    float4 result = read_imagef(top,sampler, pos);"
"    write_imagef(output, pos, result);"
"}"
"";


    sources.push_back({kernel_code.c_str(), kernel_code.length()});

    cl::Program program(context, sources);

    if(program.build({default_device}) != CL_SUCCESS) {
        cout << "Failed to build" << endl;
        cout << "******************************" << std::endl;
        cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << endl;
        cout << "******************************" << std::endl;
        cout << endl;
        return 1;
    } else {
        cout << "Successfully built code" << endl;
    }

    Image image(context, "test_00.bmp");
    image.save("output.result.bmp");
    Image second(context, "test_150.bmp");
    Image output(context, image.getWidth(), image.getHeight());

    cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * 10);

    int A[] = {0,1,2,3,4,5,6,7,8,9};
    int B[] = {0,1,2,3,4,5,6,7,8,60};

    cl::CommandQueue queue(context, default_device);

    queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
    queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);
    
    image.enqueueWrite(queue);
    second.enqueueWrite(queue);

    // cl::KernelFunctor simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
    // Replaced functor with:
    // cl::Kernel simple_add(program, "simple_add");
    // simple_add.setArg(0, buffer_A);
    // simple_add.setArg(1, buffer_B);
    // simple_add.setArg(2, buffer_C);
    // queue.enqueueNDRangeKernel(simple_add, cl::NullRange, cl::NDRange(10), cl::NullRange);
    // queue.finish();

    cl::Kernel subtract(program, "image_subtract");
    subtract.setArg(0, buffer_A);
    subtract.setArg(1, buffer_B);
    subtract.setArg(2, buffer_C);
    // queue.enqueueNDRangeKernel(subtract, cl::NullRange, cl::NDRange(10), cl::NullRange);

    int C[10];

    output.enqueueRead(queue);
    output.save("output.bmp");
    // queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 10, C);

    // cout << "Results in C:" << endl;
    // for(int i = 0; i < 10; i++){
    //     std::cout << C[i] << endl;
    // }

    cout << "Done" << endl;

    return 0;
}