/*    DEV_Config.h
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface
 *
 */

#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_


#include <termio.h>
#include <stdio.h>
#include <stdint.h>
#include "../i2cfunc.h"


#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
* IIC 
**/
#define IIC_Addr					0x29
#define IIC_Bus						4
/**
 * delay x ms
**/
#define DEV_Delay_ms(__xms)   delay_ms(__xms)


/*-----------------------------------------------------------------------------*/
void  IIC_Write(UBYTE command_, UBYTE data_);
void IIC_SMBUS_Write(UBYTE command_, UBYTE data_);
UBYTE IIC_Read(UBYTE command_);
UBYTE IIC_SMBUS_Read(UBYTE command);
UWORD IIC_ReadWord(UBYTE command_);
UWORD IIC_SMBUS_ReadWord(UBYTE command_);

UBYTE DEV_ModuleInit(void);
void DEV_ModuleExit(void);

#endif