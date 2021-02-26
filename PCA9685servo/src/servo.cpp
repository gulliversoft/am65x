// Servo.cpp
/* Martin Shishkov 2021

Short explanation of the speed encoding/decoding
================================================
This is the digger hexadecimal telegramm:
{0,A0,B0,0,A1,B1,0,0,0,0,0,0,0,0,CR};
The telegram supports values per direction up to FF which is 255,
where for the first servo, the byte 0 is the direction, followed by the 2 speed halfbytes(nibbles).
Any speed/angle bellow of A (0-9) appears in the B0. Higher or equal than A demands the LSN byte A0.
Anyway the servo supports up to 180 discrete encodings.


  A0 is the low significant
  nibble of byte 1         B0 is the most significant nibble of the byte 1
        +--------+           +--------+
        |    XXXX|           |    YYYY|
        +-------++           +-------++
                |                    |
 A1 = A0 as u8  |     B1 = B0 as u8  |
+---------------v+   +---------------v+
|0000XXXX       |    |0000YYYY       |
+---------------++   +---------------++
                |                    |
   A2 = A1 << 4 |                    |
+---------------v+                   |
|XXXX0000       |                    |
+---------------++                   |
                |              +--+  |
                +-------------->OR<--+
                               +-++
                                 |
                     V = A2 | B1 |
                 +----------+----v+
                 |XXXXYYYY        |
                 +----------------+

all this solved by the nibbles_to_u8 macro, where LSN is A0 and MSN is B0
*/
#define  nibbles_to_u8(MSN,LSN) ((unsigned char) ((unsigned char) MSN))<<4 | ((unsigned char) LSN)

#include "servo.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include "DEV_Config.h"
#include "PCA9685.h"

const UBYTE maxpos_TILTSERVO = 120;
const UBYTE maxpos_PANSERVO = 180;
#define TILTSERVO 0
#define PANSERVO  1

int ServoOpen()
{
	if(DEV_ModuleInit() == 1)
		return 1;
	Init_PCA9685();

	PCA9685_Set_Rotation_Angle(TILTSERVO, 0);
	PCA9685_Set_Rotation_Angle(PANSERVO, 0);

	return 0;
}

int ServoWrite(uint8_t data[], bool test)
{
	unsigned int len = sizeof(data);
	int retval = 0;
	unsigned char* ptSpeedPANSERVO = data + 1;
	unsigned char* ptSpeedTILTSERVO = data + 4;
	unsigned char* ptExecutionState = &data[len - 1];
	bool servoOpenRequest =  data[len - 2] > 0 ? true : false;
	data[len - 2] = 0;

	if(len != TELEGRAMSIZE)
	{
		printf("\nSERVO telegram length wrong\n");
		return DISABLED;
	}
	if(*ptExecutionState == CR)
	{
		return retval;
	}
	*ptExecutionState = CR;

	unsigned char lsn = *ptSpeedPANSERVO & 0x0F;
	unsigned char msn = *(ptSpeedPANSERVO + 1) & 0x0F;
	unsigned char cmd = nibbles_to_u8(msn, lsn);
	unsigned char origin = maxpos_PANSERVO/2;
	unsigned char dir = *(ptSpeedPANSERVO - 1) & 0x0F;
	UBYTE angle = dir == 0 ? cmd + origin : std::abs(cmd - origin);
	angle = angle > maxpos_PANSERVO ? (dir == 0 ? maxpos_PANSERVO : 0) : angle;
	fprintf(stderr, "Servo %d, dir %d, rot %d, cmd %d\n", PANSERVO, dir, angle, cmd);
	if(!test)
	{
		if(servoOpenRequest)
			ServoOpen();
		PCA9685_Set_Rotation_Angle(PANSERVO, angle);
	}

	lsn = *ptSpeedTILTSERVO & 0x0F;
	msn = *(ptSpeedTILTSERVO + 1) & 0x0F;
	cmd = nibbles_to_u8(msn, lsn);
	origin = maxpos_TILTSERVO/2;
	dir = *(ptSpeedTILTSERVO - 1) & 0x0F;
	angle = dir == 0 ? cmd + origin : std::abs(cmd - origin);
	angle = angle > maxpos_TILTSERVO ? (dir == 0 ? maxpos_TILTSERVO : 0) : angle;
	fprintf(stderr, "Servo %d, dir %d, rot %d, cmd %d\n", TILTSERVO, dir, angle, cmd);
	if(!test)
	{
		PCA9685_Set_Rotation_Angle(TILTSERVO, angle);
	}

	return retval;
}


void ServoClose()
{
	IIC_Write(MODE2,0x00);
	DEV_ModuleExit();
}
