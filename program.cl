void kernel simple_add(global const int* A, global const int* B, global int* C){
    C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];
}


const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
void kernel image_subtract(
    read_only image2d_t top,
    read_only image2d_t bottom,
    write_only image2d_t output) {
    const int2 pos = {get_global_id(0), get_global_id(1)};
    float4 result = read_imagef(top,sampler, pos) - read_imagef(bottom, sampler, pos);
    write_imagef(output, pos, result);
}