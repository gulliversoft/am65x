#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <signal.h>
#include <string.h>

#include "DEV_Config.h"
#include "PCA9685.h"

void  Handler(int signo)
{
	//System Exit
	printf("\r\nHandler:Program stop\r\n"); 

	IIC_Write(MODE2,0x00);
	DEV_ModuleExit();

	exit(0);
}

void demo()
{
	printf("\r\nMake demo\r\n");
	PCA9685_Set_Rotation_Angle(0, 80);
	while(1)
	{
		for(int i = 10; i < 170; i = i + 1){
			fprintf(stderr, "Servo 1, rotation %d\n", i);
			PCA9685_Set_Rotation_Angle(1, i);
			if(i < 80)
			{
				PCA9685_Set_Rotation_Angle(0, i);
				fprintf(stderr, "Servo 0, rotation %d\n", i);
			}
			DEV_Delay_ms(25);
		}
		for(int i = 170; i > 10; i = i - 1){
			PCA9685_Set_Rotation_Angle(1, i);
			fprintf(stderr, "Servo 1, rotation %d\n", i);
			if(i < 80)
			{
				PCA9685_Set_Rotation_Angle(0, i);
				fprintf(stderr, "Servo 0, rotation %d\n", i);
			}
			DEV_Delay_ms(25);
		}
	}
}

unsigned char check_d_parameter(int argc, char **argv)
{
	for (int i = 0; i < argc; i++)
	{
		fprintf(stderr, "argv%d %s\n", i, argv[i]);
		if (0 == strcmp("-d", argv[i]))
		{
			return 1;
		}
	}
	printf("There is no parameter set. Use -d for the demo.\r\n");
	return 0;
}

int main(int argc, char **argv)
{
	if (DEV_ModuleInit() == 1)
		return 1;

	// Exception handling:ctrl + c
	signal(SIGINT, Handler);
	//PCA9685 initialization
	Init_PCA9685();

	printf("Parking position of the servos.\r\n");
	PCA9685_Set_Rotation_Angle(0, 0);
	PCA9685_Set_Rotation_Angle(1, 0);

	if(check_d_parameter(argc, argv) == 1)
	{
		demo();
	}

	DEV_ModuleExit();
	return 0;
}