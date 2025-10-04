#include <stdio.h>
#include <SystemOut.h>
#include "stm32f1xx_hal.h"
#include <usbd_midi_if.h>

bool usart1send(char b)
{
	if((USART1->SR & USART_SR_TXE) == 0)
	{
		return false;
	}

	USART1->DR = (uint8_t)b;

	return true;
}

SystemOut sysOut(usart1send);

extern "C"
void sysOutSend(char *buf, uint32_t length)
{
	sysOut.send(buf, length);
}

uint32_t dataReceived = 0;
char tmpbuf[32];

extern "C"
int appMain(void)
{
	//printf("Hello world\n\r");

	while(true)
	{
		if(dataReceived){
			printf("%s", tmpbuf);
			fflush(stdout);
			uint8_t tmp[9]={'o','d','e','b','r','a','n','o','\n'};
			MIDI_Transmit_FS(tmp, 9);
			//printf("received\n");
			dataReceived = 0;
		}

		sysOut.tick();
	}
}
