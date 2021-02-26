/*    am65x.c
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define AM65X_LIBRARY_BUILD
#include "am65x.h"
#include "i2cfunc.h"


int dev_handle = 0;

unsigned int am65x_version(void) 
{
    return AM65X_VERSION;
}


int  am65x_i2c_begin(unsigned char bus, unsigned char addr)
{
    dev_handle = i2c_open(bus, addr);
	return dev_handle;
}

void  am65x_i2c_end(void)
{
    i2c_close(dev_handle);
}

/* Writes an number of bytes to I2C using the plain interface*/
uint8_t  am65x_i2c_write(unsigned char * buf, uint32_t len)
{
	if (i2c_write(dev_handle, buf, len) == len)
	{
		return AM65X_I2C_REASON_OK;
	}
	return AM65X_I2C_REASON_ERROR_NACK;
}

/* Writes an number of bytes to I2C using the recommended smbus interface*/
uint8_t  am65x_i2c_smbus_write(unsigned char * buf, uint32_t len)
{
	if (i2c_smbus_write(dev_handle, buf, len) == len)
	{
		return AM65X_I2C_REASON_OK;
	}
	return AM65X_I2C_REASON_ERROR_NACK;
}

/* Read an number of bytes from I2C using the plain interface*/
uint8_t  am65x_i2c_read(unsigned char* buf, uint32_t len)
{
    if (i2c_read(dev_handle, buf, len) == len)
	{
		return AM65X_I2C_REASON_OK;
	}
	return AM65X_I2C_REASON_ERROR_NACK;
}

/* Read an number of bytes from I2C using the recommended smbus interface*/
uint8_t  am65x_i2c_smbus_read(unsigned char* buf, uint32_t len)
{
    int result = i2c_smbus_read(dev_handle, buf, len);
	if (result >= 0)
	{
		fprintf(stderr, "am65x reads %d\n", result);
		return AM65X_I2C_REASON_OK;
	}
	return AM65X_I2C_REASON_ERROR_NACK;
}


/* Initialise this library. */
int  am65x_init(void)
{
   return 1; /* Success */
}

/* Close this library and deallocate everything */
int  am65x_close(void)
{
    return 1; /* Success */
} 



