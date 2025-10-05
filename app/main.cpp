#include <stdio.h>
#include <SystemOut.h>
#include "stm32f1xx_hal.h"
#include <usb/usbd_midi_if.h>
#include <UsbMidiSender.h>
#include <MidiReceiver.h>

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

bool usart2rec(uint8_t& b)
{
	if((USART2->SR & USART_SR_RXNE) != 0)
	{
		b = (uint8_t)(USART2->DR & 0xFF);

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

bool sendUsbMidi(USB_MIDI_EventPacket_t package)
{
	return ( USBD_OK == MIDI_Transmit_FS(package.bytes, 4) );
}

UsbMidiSender usbMidiSender(&sendUsbMidi);
MidiReceiver midiReceiver(&usart2rec, &usbMidiSender);


extern "C" int appMain(void)
{
	//printf("Hello world\n\r");

	while(true)
	{
		uint8_t b1, b2, b3;

		if(midiReceiver.nextEvent(b1, b2, b3)){
			//printf("midi\n");
		}

		char b;
		if(usart1rec(&b)) {
			if(b == 's') {
				printf("note ON\n");
				uint8_t tmp[4];
				tmp[0] = 0x90;   // Note On, channel 1
				tmp[1] = 60;     // Note number
				tmp[2] = 100;    // Velocity
				usbMidiSender.sendMidi(tmp, 3);
			}
			else if(b == 'x') {
				printf("note OFF\n");
			    uint8_t tmp[4];
			    tmp[0] = 0x90;   // Note On, channel 1
			    tmp[1] = 60;     // Note number
			    tmp[2] = 0;    // Velocity
			    usbMidiSender.sendMidi(tmp, 3);
			}
		}

		midiReceiver.tick();
		usbMidiSender.tick();
		sysOut.tick();
	}
}
