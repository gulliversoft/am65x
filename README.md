![am65x in action](https://github.com/gulliversoft/am65x/blob/main/doc/am65x.gif)
![github actions](https://github.com/gulliversoft/am65x/blob/main/doc/workflows.png)

# am65x

This library is designed to be free for every one and easy-to-use, it is quite simple and providing reasonably speedy I/O.
**am65x** is a userspace programming library for accessing all devices on an adapter from userspace, through the /dev interface.

In order to manage this interface, your OS needs to load the module i2c-dev at the boot time.
In this case each registered i2c adapter gets a number, counting from 0. You can examine /sys/class/i2c-dev/ to see what number corresponds to which adapter.
Alternatively, you can run "i2cdetect -l" to obtain a formatted list of all i2c adapters present on your system at a given time. i2cdetect is part of the i2c-tools package.

In the current release am65x links statically libi2c. This binary was extracted from the debian package libi2c-dev, version 4.1.1. Since am65x wraps around libi2c, it follows the same license model [copyright](https://metadata.ftp-master.debian.org/changelogs//main/i/i2c-tools/i2c-tools_4.1-1_copyright)
For details see [libi2c-dev page](https://packages.debian.org/de/buster/libi2c-dev)
You do not need to install libi2c-dev, because am65x contains it.

Currently with two demo sources, one regarding the light sensor TSL2581 and second for controlling servo motors by using PCA9685.
In this release the I/O keeps limited to the scope of I2C functionality.
The exercises PCA9685 and TSL2581 shall lead you through the basic concepts of am65x. Each individual configuration is taking place into separated device folder, where the DEV_Config.h describes the hardware underlying interface.

## Build information:
This library is a linux user space library and compiles for different ARM flavours and once for Intel/AMD x86_64. This multi platform support demands differentiation between native build system and upper level CMake generators.
The build.sh is the script which you need to use for building. Each CMakeLists.txt describes the rules and sources behind. The Ninja is leveraged as CMake generator which creates the build.ninja in the release folder. In the next step Ninja launches to the native GNU gcc compiler and linker. 

##The demo projects:
The project TSL2581 controls the light-to-digital converter that transforms the light intensity to a digital signal output. This device combines one broadband photodiode (visible plus infrared) and one infrared-responding photodiode on a single CMOS integrated circuit.

`1 bytes request`  
`am65x reads 0`  
`READ ID = 0xD0`  
`        ---- i2c sensor init ----       `  
`=========My latest SMBUS method=========`  
`3 bytes request`  
`am65x reads 55`  
`3 bytes request`  
`am65x reads 11`  
`Ch0 = 14080`  
`Ch1 = 2816`  
`=========My plain method=========`  
`Ch0 = 2816`  
`Ch1 = 2816`  
`=========Original method=========`  
`Ch0 = 2827`  
`Ch1 = 2827`  
`1 bytes request`  
`am65x reads 0`  
`READ ID = 0xD0`  

The project PCA9685 servo communicates with the PWM driver PCA9685. The code demonstrates 2-channel 12-bit PWM/Servo Driver.

Just calling PCA9685servo simple parks the servos to the zero position. The real demo starts with -d option **PCA9685servo -d**

`Make demo`  
`Servo 1, rotation 10`  
`Servo 0, rotation 10`  
`Servo 1, rotation 11`  
`Servo 0, rotation 11`  
`Servo 1, rotation 12`  
`Servo 0, rotation 12`  
`Servo 1, rotation 13`  
`Servo 0, rotation 13`  
`Servo 1, rotation 14`  
`Servo 0, rotation 14`  
`Servo 1, rotation 15`  
`Servo 0, rotation 15`  
`Servo 1, rotation 16`  
`Servo 0, rotation 16`  
`Servo 1, rotation 17`  
`Servo 0, rotation 17`  
`Servo 1, rotation 18`  
`Servo 0, rotation 18`  
`Servo 1, rotation 19`  
`Servo 0, rotation 19`  
`Servo 1, rotation 20`  
`Servo 0, rotation 20`  
`Servo 1, rotation 21`  
`Servo 0, rotation 21`  
`Servo 1, rotation 22`  
`Servo 0, rotation 22`  

