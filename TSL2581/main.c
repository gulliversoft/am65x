#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <signal.h>

#include "DEV_Config.h"
#include "TSL2581.h"

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Program stop\r\n"); 
    
    DEV_ModuleExit();

    exit(0);
}

int main(int argc, char **argv)
{
	//int  lux;

	if (DEV_ModuleInit()==1)return 1;
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
	//because The lower four bits are the silicon version number
	printf("READ ID = 0x%02X\n\t", Read_ID() & 0xf0);
	
	printf("---- i2c sensor init ----\n");
    //TSL2581 initialization
	Init_TSL2581();
	
	while(1)
	{		
		//lux  =  calculateLux(2, NOM_INTEG_CYCLE);
		printf("=========My latest SMBUS method=========\r\n");
		Read_SMBUS_BothChannel();
		printf("Ch0 = %d \r\n", Channel_0);
		printf("Ch1 = %d \r\n", Channel_1);
		printf("=========My plain method=========\r\n");
		Read_BothChannel();
		printf("Ch0 = %d \r\n", Channel_0);
		printf("Ch1 = %d \r\n", Channel_1);
		printf("=========Original method=========\r\n");
		Read_Channel();
		printf("Ch0 = %d \r\n", Channel_0);
		printf("Ch1 = %d \r\n", Channel_1);
		//printf("lux = %d \r\n", lux);
		printf("READ ID = 0x%02X\r\n", Read_ID() & 0xf0);
		Init_TSL2581();
		DEV_Delay_ms(999);
	}

	DEV_ModuleExit();
    return 0; 
}