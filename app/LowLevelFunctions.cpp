#include <LowLevelFunctions.h>
#include <stm32f1xx_hal.h>
#include <SystemOut.h>
#include <usb/usbd_midi_if.h>
#include <MidiUsbReceiver.h>
#include <MidiSerialReceiver.h>


extern SystemOut sysOut;
extern MidiUsbReceiver midiUsbReceiver;
extern MidiSerialReceiver midiSerialReceiver;

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

bool usart2send(uint8_t b)
{
	if((USART2->SR & USART_SR_TXE) == 0)
	{
		return false;
	}

	USART2->DR = (uint8_t)b;

	return true;
}

extern "C"
void usart2Interrupt()
{
	uint8_t b;

	if(usart2rec(b)) {
		midiSerialReceiver.newUartByte(b);
	}
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

extern "C"
void newUsbMidiData(uint8_t* buf, uint32_t len)
{
	UsbMidiEventPacket packet;

	for(uint32_t i=0; i<len; i+=4) {
		bool isValidPacket = ((len - i) > 3) && ( (buf[i] & 0x0F) <= 0x0F && (buf[i+1] & 0x80) );

		if(isValidPacket) {
			packet.bytes[0] = buf[i];
			packet.bytes[1] = buf[i+1];
			packet.bytes[2] = buf[i+2];
			packet.bytes[3] = buf[i+3];

			midiUsbReceiver.newUsbEvent(packet);
		}
	}
}

