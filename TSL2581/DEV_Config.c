/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   
* | Function    :   Hardware underlying interface
* | Info        :

******************************************************************************/
#include "DEV_Config.h"
#include "../am65x.h"
#include "TSL2581.h"
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
 Accepts COMMAND_READ_CH0 or COMMAND_READ_CH1 and reads the ADC Channels using Read Word Protocol, which is 
 the RECOMMENDED way.
 By using Command 0xb4 for the channel 0. these two bytes are readen from sequential registers 0x14 and 0x15
 By using Command 0xb6 for the channel 1. these two bytes are readen from sequential registers 0x16 and 0x17
******************************************************************************/
UWORD IIC_ReadWord(UBYTE command_)
{
	if((command_ != COMMAND_READ_CH0) && (command_ != COMMAND_READ_CH1))
	{
		printf("Wrong command passed by IIC_ReadWord\n");
		return 0;
	}
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
 Accepts COMMAND_READ_CH0 or COMMAND_READ_CH1 and reads the ADC Channels using Read Word Protocol, which is 
 the RECOMMENDED way.
 By using Command 0xb4 for the channel 0. these two bytes are readen from sequential registers 0x14 and 0x15
 By using Command 0xb6 for the channel 1. these two bytes are readen from sequential registers 0x16 and 0x17
******************************************************************************/
UWORD IIC_SMBUS_ReadWord(UBYTE command_)
{
	if((command_ != COMMAND_READ_CH0) && (command_ != COMMAND_READ_CH1))
	{
		printf("Wrong command passed by IIC_SMBUS_ReadWord\n");
		return 0;
	}
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
