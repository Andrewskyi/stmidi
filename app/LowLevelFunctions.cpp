#include <LowLevelFunctions.h>
#include <stm32f1xx_hal.h>
#include <SystemOut.h>
#include <usb/usbd_midi_if.h>

extern SystemOut sysOut;

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

extern "C"
void sysOutSend(char *buf, uint32_t length)
{
	// yes sometimes buffer could be overloaded, it is for debug only, so it don't need to be very reliable
	sysOut.write(buf, length);
}

bool sendUsbMidi(UsbMidiEventPacket package)
{
	return ( USBD_OK == MIDI_Transmit_FS(package.bytes, 4) );
}
