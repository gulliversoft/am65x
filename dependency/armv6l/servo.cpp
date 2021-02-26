// Servo.cpp
// Martin Shishkov 2020
#include "../../PCA9685servo/servo.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <chrono>
#include <thread>


int sfd = DISABLED; /* File descriptor for the port */

//-------------- converts a hex value (00..0F) into the ASCII-character '0'..'F' -------------------
unsigned char hex2asc(unsigned char hexbyte)
{

    hexbyte += 48;          // ASCII code for 0-9 is 48-57

    if(hexbyte>=58)         // ASCII code for A-F is 65-70 -> in that case add 7
        hexbyte += 7;

    return hexbyte;
}

int ServoTest()
{
	int retval = 0;
	const unsigned char demo[] = {0,'A','A',0,'8',0,0,0,0,0,0,0,0,0,CR};
	const unsigned int len = sizeof (demo);
	if(len != TELEGRAMSIZE)
	{
		printf("\nSERVO Test: Telegram length wrong\n");
		return DISABLED;
	}
	printf("\nSERVO Test: ");
	for(int i = 0; i < len; i++)
	{
		retval += write(sfd, &demo[i], 1);
		printf(",0x%x", demo[i]);
	}

	if(retval != len)
	{
		perror("\nSERVO Test: Unable to write\n");
	}
	else
	{
		printf("\nSERVO Test OK\n");
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE));
	return retval;
}

int ServoOpen()
{
	if(sfd > 0)
		return DISABLED;
	sfd = open("/dev/serial0",  O_RDWR | O_NOCTTY | O_NDELAY );
	if (sfd == DISABLED)
	{
		perror("\nServo: open_port: Unable to open /dev/serial0 - ");
		return DISABLED;
	};


	struct termios options;

	/*
	 * Get the current options for the port...
	 */

	tcgetattr(sfd, &options);

	/*
	 * Set the baud rates to 9600...
	 */

	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	/*
	 * Enable the receiver and set local mode...
	 */

	options.c_cflag |= (CLOCAL | CREAD);


	//No parity (8N1):
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	/*
	 * Set the new options for the port...
	 */

	tcsetattr(sfd, TCSANOW, &options);


	return sfd;
}

int ServoWrite(UserDataStruct* data, bool test)
{
	if(test)
	{
		return ServoTest();	
	}	
	unsigned int len = sizeof(data->diggerData);
	data->diggerData[len - 1] = CR;
	int retval = 0;
	//int retval = write(sfd, data->diggerData, len);
	for(int i = 0; i < len; i++)
	{
		const unsigned char hex = hex2asc(data->diggerData[i] & 0x0F);
		retval += write(sfd, &hex, 1);
	}
	if(retval != len)
	{
		perror("\nServo: Unable to write\n");
	}
	return retval;
}


void ServoClose()
{
	close(sfd);
	sfd = DISABLED;
}

