/*    i2cfunc.c
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface
 *
 */

#include <stdio.h>
#include "smbus.h"
#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "i2cfunc.h"

#ifndef I2C_SLAVE
#define I2C_SLAVE 0x0703
#endif

/******************************************************************************
function:
i2c_open
uses character device file interface
******************************************************************************/
int i2c_open(unsigned char bus, unsigned char addr)
{
	int file;
	char filename[16];
	sprintf(filename,"/dev/i2c-%d", bus);
	if ((file = open(filename,O_RDWR)) < 0)
	{
		fprintf(stderr, "i2c_open open error: %s\n", strerror(errno));
		return(file);
	}
	if (ioctl(file, I2C_SLAVE, addr) < 0)
	{
		fprintf(stderr, "i2c_open ioctl error: %s\n", strerror(errno));
		return(-1);
	}
	return(file);
}

int i2c_write(int handle, unsigned char* buf, unsigned int length)
{
	if (write(handle, buf, length) != length)
	{
		fprintf(stderr, "i2c_write error: %s\n", strerror(errno));
		return(-1);
	}
	return(length);
}

/******************************************************************************
function:
i2c_smbus_read
SMBus commands are preferred if the device supports them.
Length of 0 reads out single byte, without command set.
I any other case the required command needs to be placed in the first byte of the buffer parameter.
The result is returned as value, the same fills the buffer back, splitted bytewise

Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
ask for more than 2 and less than 32 bytes, your code will only work with kernels 2.6.23 and later.
https://bytefreaks.net/tag/i2c_smbus_read_byte_data
******************************************************************************/
int i2c_smbus_read(int handle, unsigned char* buf, unsigned int length)
{
	fprintf(stderr, "%d bytes request\n", length);
	//in the zero case it is returning negative errno else the byte received from the device. No command required.
	if(length == 0)
	{
		int result = i2c_smbus_read_byte(handle);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_read byte error: %s, %d\n", strerror(errno), result);
			return result;
		}
		return result;
	}
	//in the first case it is returning negative errno else the byte received from the device, this time acccording with the command.
	if(length == 1 + 1)
	{
		int result = i2c_smbus_read_byte_data(handle, buf[0]);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_read byte error: %s, %d\n", strerror(errno), result);
			return result;
		}
		buf[0] = (unsigned char)(result & 0x000000ff);
		if(buf[0] != result)
		{
			fprintf(stderr, "i2c_smbus_read byte shift error: %d, %d\n", result, buf[0]);
			return -1;
		}
		return result;
	}
	//in the second case it is returning negative errno else a 16-bit unsigned “word” received from the device
	else if(length == 1 + 2)
	{
		int result = i2c_smbus_read_word_data(handle, buf[0]);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_read word data error: %s, %d\n", strerror(errno), result);
			return result;
		}
		buf[0] = (result & 0x000000ff);
		buf[1] = (result & 0x0000ff00) >> 8;
		return result;
	}
	//returning negative errno else the number of read bytes received from the device. The values from the response
	//back in the buffer. SMBus allows at most 32 bytes.
	else
	{
		int result = i2c_smbus_read_block_data(handle, buf[0], buf);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_read block data error: %s, %d\n", strerror(errno), result);
		}
		return result;
	}
	return -1;
}

/******************************************************************************
function:
i2c_smbus_write
SMBus commands are preferred if the device supports them.
Length of 0 is not accepted.
The lenght 1 means passing byte only without command.
Any other lenght assumes that the required command appears in the first buffer byte, followed by the payload bytes.
The result is zero by success or errornumber returned.
******************************************************************************/
int i2c_smbus_write(int handle, unsigned char* buf, unsigned int length)
{
	if(length == 0)
	{
		return -1;
	}
	else if(length == 1)
	{
		int result = i2c_smbus_write_byte(handle, buf[0]);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_write byte error: %s, %d\n", strerror(errno), result);
			return result;
		}
		return result;
	}
	else if(length == 2)
	{
		int result = i2c_smbus_write_byte_data(handle, buf[0], buf[1]);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_write byte data error: %s, %d\n", strerror(errno), result);
			return result;
		}
		return result;
	}
	else if(length == 3)
	{
		int result = i2c_smbus_write_word_data(handle, buf[0], bytes_to_u16(buf[2], buf[1]));
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_write word data error: %s, %d\n", strerror(errno), result);
			return result;
		}
		return result;
	}
	else
	{
		int result = i2c_smbus_write_block_data(handle, buf[0], length, buf + 1);
		if(result < 0)
		{
			fprintf(stderr, "i2c_smbus_write block data error: %s, %d\n", strerror(errno), result);
		}
		return result;
	}
	return -1;
}

/******************************************************************************
function:
i2c_read
Uses plain I2C instead of the preferred SMBus command interface
******************************************************************************/
int i2c_read(int handle, unsigned char* buf, unsigned int length)
{
	if (read(handle, buf, length) != length)
	{
		fprintf(stderr, "i2c_read error: %s\n", strerror(errno));
		return(-1);
	}
	return(length);
}


int i2c_close(int handle)
{
	if ((close(handle)) != 0)
	{
		fprintf(stderr, "i2c_close error: %s\n", strerror(errno));
		return(-1);
	}
	return(0);
}


int delay_ms(unsigned int msec)
{
	int ret;
	struct timespec a;
	if (msec > 999)
	{
		fprintf(stderr, "delay_ms error: delay value needs to be less than 999\n");
		msec = 999;
	}
	a.tv_nsec=((long)(msec)) * 1000000.0;
	a.tv_sec=0;
	if ((ret = nanosleep(&a, NULL)) != 0)
	{
		fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
	}
	return(0);
}



