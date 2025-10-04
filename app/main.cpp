#include <stdio.h>
#include <SystemOut.h>
#include "stm32f1xx_hal.h"
#include <usb/usbd_midi_if.h>

bool usart1send(char b)
{
	if((USART1->SR & USART_SR_TXE) == 0)
	{
		return false;
	}

	USART1->DR = (uint8_t)b;

	return true;
}

bool usart1rec(char* b)
{
	if((USART1->SR & USART_SR_RXNE) != 0)
	{
		*b = (uint8_t)(USART1->DR & 0xFF);

		return true;
	}

	return false;
}

SystemOut sysOut(usart1send);

extern "C"
void sysOutSend(char *buf, uint32_t length)
{
	// yes sometimes buffer could be overloaded, it is for debug only, so it don't need to be very reliable
	sysOut.write(buf, length);
}

uint32_t dataReceived = 0;
char tmpbuf[32];

extern "C"
int appMain(void)
{
	//printf("Hello world\n\r");

	while(true)
	{
		char b;
		if(usart1rec(&b)) {
			if(b == 'a') {
				printf("note ON\n");
				uint8_t tmp[4];
				tmp[0] = 0x09;   // CN=0, CIN=0x9 (Note On)
				tmp[1] = 0x90;   // Note On, channel 1
				tmp[2] = 60;     // Note number
				tmp[3] = 100;    // Velocity
				MIDI_Transmit_FS(tmp, 4);
			}
			else if(b == 'z') {
				printf("note OFF\n");
			    uint8_t tmp[4];
			    tmp[0] = 0x08;   // CN=0, CIN=0x9 (Note Off)
			    tmp[1] = 0x90;   // Note On, channel 1
			    tmp[2] = 60;     // Note number
			    tmp[3] = 0;    // Velocity
			    MIDI_Transmit_FS(tmp, 4);
			}
		}

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
