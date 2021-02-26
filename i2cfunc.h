/*    i2cfunc.h
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface with func prototypes for I2C
 *
 */
#define bytes_to_u16(MSB,LSB) (((unsigned int) ((unsigned char) MSB)) & 255)<<8 | (((unsigned char) LSB)&255)

// bus=4 for interface I2C on x10 of IOT2050A
// returns handle to be used in remainder functions
// addr is a 7-bit value (so, for example the BMP085 datasheet specifies
// 0xEE, so we need to right-shift by 1 and use 0x77 for this function)
int i2c_open(unsigned char bus, unsigned char addr);

// These functions return -1 on error, otherwise return the number of bytes read/written.
// To perform a 'repeated start' use the i2c_write_read function which can write some
// data and then immediately read data without a stop bit in between.
int i2c_write(int handle, unsigned char* buf, unsigned int length);
int i2c_read(int handle, unsigned char* buf, unsigned int length);

int i2c_write_byte(int handle, unsigned char val);
int i2c_read_byte(int handle, unsigned char* val);

// These functions return -1 on error, otherwise return 0 on success
int i2c_close(int handle);
// Provides an inaccurate delay (may be useful for waiting for ADC etc).
// The maximum delay is 999msec
int delay_ms(unsigned int msec);

/******************************************************************************
function:
i2c_smbus_read
SMBus commands are preferred if the device supports them.
Length of 0 reads out single byte, without command set.
I any other case the required command needs to be placed in the first byte of the buffer parameter.
The result is returned as value, the same fills the buffer back, splitted bytewise

Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
ask for more than 2 and less than 32 bytes, your code will only work with kernels 2.6.23 and later.
******************************************************************************/
int i2c_smbus_read(int handle, unsigned char* buf, unsigned int length);

/******************************************************************************
function:
i2c_smbus_write
SMBus commands are preferred if the device supports them.
The required command need to be placed in the first byte of the buffer parameter.
The writing value, which needs bytesplit is placed bellow the byte 0
The result is zero by success
******************************************************************************/
int i2c_smbus_write(int handle, unsigned char* buf, unsigned int length);

