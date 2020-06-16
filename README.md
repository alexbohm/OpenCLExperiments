## OpenCLExperiments
A repo for experimenting with OpenCL on various devices.

### Operating System Setup ###
For this project I am using the Odroid XU4/Odroid HC2.
* Installation Instructions
    - Since I am using the Odroid HC2, I followed these instructions:
    - https://archlinuxarm.org/platforms/armv7/samsung/odroid-hc2
    - If you use the Odroid XU4, try these instructions:
    - https://archlinuxarm.org/platforms/armv7/samsung/odroid-xu4
* Pacman Packages
    - base-devel
    - odroid-xu3-libgl-fb
    - ocl-icd
    - opencl-headers
    - clinfo

### System Setup ###
* Allowing users with video privileges to run OpenCL code
    - The Mali GPU appears to be accessible on `/dev/mali0`. By default on
installation only root can read and write this device. Let's change that so a
less privileged user can access the GPU.
    - `sudo chgrp video /dev/mali0`
    - `sudo chmod g+rw /dev/mali0`
    - `sudo usermod -aG `
    - Log out and log back in again
* 

### Troubleshooting ###
* If you are getting information similar to the output below,
allow the `video` group to read and write `/dev/mali0`.
Be sure to add your user to the `video` group and re-log to refresh the user privileges.
- ```$ ./subtract 
./subtract: /usr/lib/mali-egl/libOpenCL.so.1: no version information available (required by ./subtract)
./subtract: /usr/lib/mali-egl/libOpenCL.so.1: no version information available (required by ./subtract)
Found 1 Platforms:
    ARM Platform
ERROR: The DDK is not compatible with any of the Mali GPUs on the system.
The DDK was built for 0x620 r0p1 status range [0..15], but none of the GPUs matched:
Found 0 Devices:
Segmentation fault (core dumped)
```
- This will also affect `clinfo`
### Useful Resources ###
- The arch linux wiki gives some applicable knowledge for OpenCL
- https://wiki.archlinux.org/index.php/GPGPU