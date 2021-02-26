// Servo.h
// Martin Shishkov 2020
#include <stdint.h>
int ServoOpen();
int ServoWrite(uint8_t data[], bool test);
void ServoClose();

#define CR 13		// carriage return ASCII=13
#define DISABLED -1 //RS232 return value by non existing port
#define TELEGRAMSIZE 15

#define CYCLE 150
//#define TEST

#ifdef TEST
	#undef CYCLE
	#define CYCLE 500
#endif

const unsigned char testdata[] = {
		0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,0x10,0x11,0x12,0x13,0x14,0x15};



