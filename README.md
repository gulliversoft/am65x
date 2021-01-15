# am65x

This library is designed to be free for every one and easy-to-use, it is quite simple and providing reasonably speedy I/O.
am65x is a userspace programming library for accessing all devices on an adapter from userspace, through the /dev interface.

In order to manage this interface, your OS needs to load the module i2c-dev at the boot time.
In this case each registered i2c adapter gets a number, counting from 0. You can examine /sys/class/i2c-dev/ to see what number corresponds to which adapter.
Alternatively, you can run "i2cdetect -l" to obtain a formatted list of all i2c adapters present on your system at a given time. i2cdetect is part of the i2c-tools package.

In the current release am65x is a wrapper around of libi2c-dev.
am65x library follows the license model of the parent https://metadata.ftp-master.debian.org/changelogs//main/i/i2c-tools/i2c-tools_4.1-1_copyright
For details see https://packages.debian.org/de/buster/libi2c-dev
Before start with am65x you need to solve the dependency by calling: sudo apt-get install libi2c-dev.

Currently with two demo sources, one regrading the light sensor TSL2581 and second for controlling servo motors by using PCA9685.
In this release limited to the scope of I2C functionality.
Each individual configuration is taking place into separated device folder, where the DEV_Config.h describes the hardware underlying interface.
