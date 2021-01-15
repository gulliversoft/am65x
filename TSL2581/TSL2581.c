/*    TSL2581.c
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface
 *
 */

#include "TSL2581.h"


UBYTE Read_ID()
{
	//return IIC_Read(COMMAND_CMD |TRANSACTION |ID);
	return IIC_SMBUS_Read(COMMAND_CMD |TRANSACTION |ID);
}
 
/**********************************************************************************************
* @brief  	Init_TSL2581()
* @param   	power on ,set gain is 16,interrupt is 688ms
* @param   
* @param    This field selects the integration time for each conversion.
**********************************************************************************************/
void Init_TSL2581(void)
{
	IIC_SMBUS_Write(COMMAND_CMD, CONTROL_POWERON);//x80, Power_On = 0x01
	DEV_Delay_ms(2);//Wait 2 ms power on
	IIC_SMBUS_Write(COMMAND_CMD | CONTROL, INTEGRATIONTIME_688MS);//x81, interrupt ITIME = 688 ms
	IIC_SMBUS_Write(COMMAND_CMD, ADC_EN | CONTROL_POWERON);//x80, enable ADC Channels by keeping the device powered on
}


/**********************************************************************************************
* @brief  	Read_Channel() // plain Read of the ADC Channels
**********************************************************************************************/
void Read_Channel()
{	
	UBYTE DataLow,DataHigh;
	DataLow = IIC_Read(COMMAND_CMD | TRANSACTION | DATA0LOW);
	DataHigh = IIC_Read(COMMAND_CMD | TRANSACTION | DATA0HIGH);
	Channel_0 = 256 * DataHigh + DataLow ;
		
	DataLow = IIC_Read(COMMAND_CMD | TRANSACTION | DATA1LOW);
	DataHigh = IIC_Read(COMMAND_CMD | TRANSACTION | DATA1HIGH);
	Channel_1 = 256 * DataHigh + DataLow ;
	
}


/**********************************************************************************************
* @brief  	Read_BothChannel() // plain Read of the ADC Channels
**********************************************************************************************/
void Read_BothChannel()
{	
	Channel_0 = IIC_ReadWord(COMMAND_READ_CH0);
	Channel_1 = IIC_ReadWord(COMMAND_READ_CH1);
}

/**********************************************************************************************
* @brief  	Read_SMBUS_BothChannel() // Read ADC Channels Using Read Word Protocol - RECOMMENDED
**********************************************************************************************/
void Read_SMBUS_BothChannel()
{	
	Channel_0 = IIC_SMBUS_ReadWord(COMMAND_READ_CH0);
	Channel_1 = IIC_SMBUS_ReadWord(COMMAND_READ_CH1);
}

/**********************************************************************************************
* @brief  	calculateLux()
* @param    Channel_0 and Channel_1 is for TSL2561_Read_Channel();
* @param   	// Arguments: unsigned int iGain - gain, where 0:1X, 1:8X, 2:16X, 3:128X
* @param   	// unsigned int tIntCycles - INTEG_CYCLES defined in Timing Register
**********************************************************************************************/
UDOUBLE calculateLux(UWORD iGain, UWORD tIntCycles)
{
	UDOUBLE chScale0;
	UDOUBLE chScale1;
	UDOUBLE channel1;
	UDOUBLE channel0;
	UDOUBLE temp;
	UDOUBLE ratio1 = 0;
	UDOUBLE ratio;
	UDOUBLE lux_temp;
	UWORD b, m;

	// No scaling if nominal integration (148 cycles or 400 ms) is used
	if (tIntCycles == NOM_INTEG_CYCLE)
	{
	//     chScale0 = 65536;
	  chScale0 = (1 << (CH_SCALE));
	}
	else
	chScale0 = (NOM_INTEG_CYCLE << CH_SCALE) / tIntCycles;
	switch (iGain)
	{
	case 0: // 1x gain
	  chScale1 = chScale0; // No scale. Nominal setting
	  break;
	case 1: // 8x gain
	  chScale0 = chScale0 >> 3; // Scale/multiply value by 1/8
	  chScale1 = chScale0;
	  break;
	case 2: // 16x gain
	  chScale0 = chScale0 >> 4; // Scale/multiply value by 1/16
	  chScale1 = chScale0;
	  break;
	case 3: // 128x gain
	  chScale1 = chScale0 / CH1GAIN128X;
	  chScale0 = chScale0 / CH0GAIN128X;
	  break;
	}
	// Read Channel for ADC
	Read_Channel();
	// scale the channel values
	channel0 = (Channel_0 * chScale0) >>  CH_SCALE;
	channel1 = (Channel_1 * chScale1) >>  CH_SCALE;
	//printf(" C10= %ld  C11 =%ld \n ",channel0,channel1);
	// find the ratio of the channel values (Channel1/Channel0)
	if (channel0 != 0)
	ratio1 = (channel1 << (RATIO_SCALE + 1)) / channel0;
	ratio = (ratio1 + 1) >> 1;	  									 // round the ratio value

	if ((ratio >= 0X00) && (ratio <= K1C))
	{    b = B1C;    m = M1C;  }
	else if (ratio <= K2C)
	{    b = B2C;    m = M2C;  }
	else if (ratio <= K3C)
	{    b = B3C;    m = M3C;  }
	else if (ratio <= K4C)//276
	{    b = B4C;    m = M4C;  }
	else if (ratio > K5C)//276
	{    b = B5C;    m = M5C;  }

	temp = ((channel0 * b) - (channel1 * m));
	temp += (1 << (LUX_SCALE - 1));			// round lsb (2^(LUX_SCALE-1))
	//  temp = temp + 32768;
	lux_temp = temp >> LUX_SCALE;			// strip off fractional portion
	return (lux_temp);		  							// Signal I2C had no errors
}

