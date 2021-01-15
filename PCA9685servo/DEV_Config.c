/*    DEV_Config.c
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface
 *
 */

******************************************************************************/
#include "DEV_Config.h"
#include "../am65x.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
/******************************************************************************
function:	
	Module initialization, I2C protocol
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{	
	if(am65x_i2c_begin(IIC_Bus, IIC_Addr) > 0)
		return 0;
	return 1;
}

/******************************************************************************
function:	
	plain I2C Write
******************************************************************************/
void IIC_Write(UBYTE command_, UBYTE data_)
{
	unsigned char Buf[2] = {command_, data_};
	am65x_i2c_write(Buf, sizeof Buf);
}

/******************************************************************************
function:	
	I2C Write
******************************************************************************/
void IIC_SMBUS_Write(UBYTE command_, UBYTE data_)
{
	unsigned char Buf[2] = {command_, data_};
	am65x_i2c_smbus_write(Buf, sizeof Buf);
}

/******************************************************************************
function:	
	plain I2C Read
******************************************************************************/
UBYTE IIC_Read(UBYTE command_)
{
	unsigned char Buf[1]={command_};
	am65x_i2c_read(Buf, sizeof Buf);
	return Buf[0];
}

/******************************************************************************
function:	
	I2C Read
******************************************************************************/
UBYTE IIC_SMBUS_Read(UBYTE command_)
{
	unsigned char Buf[1]={command_};
	am65x_i2c_smbus_read(Buf, sizeof Buf);
	return Buf[0];
}


/******************************************************************************
function:
 IIC_ReadWord
******************************************************************************/
UWORD IIC_ReadWord(UBYTE command_)
{
	unsigned char ReadBuf[2]={ 0, 0};
	if(am65x_i2c_read(ReadBuf, sizeof ReadBuf) == AM65X_I2C_REASON_OK)
	{
		// 256 * DataHigh + DataLow
		return 256 * ReadBuf[0] + ReadBuf[1];
	}

	fprintf(stderr, "IIC_ReadWord error: %s\n", strerror(errno));
	return 0;
}

/******************************************************************************
function:
 IIC_SMBUS_ReadWord
******************************************************************************/
UWORD IIC_SMBUS_ReadWord(UBYTE command_)
{
	unsigned char ReadBuf[3]={command_, 0, 0};
	if(am65x_i2c_smbus_read(ReadBuf, sizeof ReadBuf) == AM65X_I2C_REASON_OK)
	{
		// 256 * DataHigh + DataLow
		return 256 * ReadBuf[0] + ReadBuf[1];
	}
	
	fprintf(stderr, "IIC_SMBUS_ReadWord error: %s\n", strerror(errno));
	return 0;
}

/******************************************************************************
function:	
	Module exits, closes I2C
******************************************************************************/
void DEV_ModuleExit(void)
{
    am65x_i2c_end();
    am65x_close();
}


/************************************************/
