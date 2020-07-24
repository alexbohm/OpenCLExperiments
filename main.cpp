
#include <CL/cl.hpp>
#include <vector>
#include <string>

#include "image.h"

#include <iostream>
using std::cout;
using std::endl;

int main(int argc, char const *argv[])
{
    if(argc < 3){
        cout << "Usage: " << argv[0] << " <image1> <image2>" << endl;
        return 1;
    }
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
    cout << "This device has a native float vector size of: " << default_device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>() << endl;
    cout << "This device has a native double vector size of: " << default_device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>() << endl;

    cl::Context context({default_device});

    cl::Program::Sources sources;

    std::string kernel_code =
"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;"
"void kernel image_subtract("
"    read_only image2d_t top,"
"    read_only image2d_t bottom,"
"    write_only image2d_t output) {"
"    const int2 pos = {get_global_id(0), get_global_id(1)};"
"    const float4 result = read_imagef(top, sampler, pos) - read_imagef(bottom, sampler, pos);"
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

    Image top(context, argv[1]);
    Image bottom(context, argv[2]);
    Image output(context, top.getWidth(), top.getHeight());

    cl::CommandQueue queue(context, default_device);
    
    top.enqueueWrite(queue);
    bottom.enqueueWrite(queue);

    int error;

    cl::Kernel subtract(program, "image_subtract", &error);
    cout << "Kernel Status: " << error << endl;

    error = subtract.setArg(0, top.getBuffer());
    cout << "Argument 0: " << error << endl;
    error = subtract.setArg(1, bottom.getBuffer());
    cout << "Argument 1: " << error << endl;

    error = subtract.setArg(2, output.getBuffer());
    cout << "    Argument 2: " << error << endl;

    cout << "Enqueue Kernel Status: " << queue.enqueueNDRangeKernel(subtract, cl::NullRange, cl::NDRange(top.getWidth(), top.getHeight()), cl::NullRange) << endl;

    // cout << "Waiting for CommandQueue to finish...";
    // queue.finish();
    // cout << " Done" << endl;

    output.enqueueRead(queue);
    output.save("output.bmp");

    cout << "Done" << endl;

    return 0;
}