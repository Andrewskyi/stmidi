/*
 *
 *  Created on: 2025
 *      Author: apaluch
 *
 *      MIT License

Copyright (c) 2025 apaluch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <LowLevelFunctions.h>
#include <stm32f1xx_hal.h>
#include <SystemOut.h>
#include <MidiUsbReceiver.h>
#include <MidiSerialReceiver.h>
#include <usb/UsbMidiClassIf.h>


extern SystemOut sysOut;
extern MidiUsbReceiver midiUsbReceiver;
extern MidiSerialReceiver midiSerialReceiver;

bool usart1send(uint8_t b)
{
	if((USART1->SR & USART_SR_TXE) == 0)
	{
		return false;
	}

	USART1->DR = b;

	return true;
}

bool usart1rec(uint8_t& b)
{
	if((USART1->SR & USART_SR_RXNE) != 0)
	{
		b = (uint8_t)(USART1->DR & 0xFF);

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

	USART2->DR = b;

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
	sysOut.write((uint8_t*)buf, length);
}

uint32_t sendUsbMidi(const UsbMidiEventPacket* part1, uint32_t len1, const UsbMidiEventPacket* part2, uint32_t len2)
{
	if(MIDI_TransmitterState() != USBD_OK) {
		return 0;
	}

	uint32_t bytesNumTotal = 0;

	for(uint32_t p=0; p<len1; p++) {
		if( (bytesNumTotal + 4) > APP_TX_DATA_SIZE ) {
			break;
		}

		for(uint32_t i=0; i<4; i++) {
			MidiTxBufferFS[bytesNumTotal] = part1[p].bytes[i];
			bytesNumTotal++;
		}
	}

	for(uint32_t p=0; p<len2; p++) {
		if( (bytesNumTotal + 4) > APP_TX_DATA_SIZE ) {
			break;
		}

		for(uint32_t i=0; i<4; i++) {
			MidiTxBufferFS[bytesNumTotal] = part2[p].bytes[i];
			bytesNumTotal++;
		}
	}

	return ( USBD_OK == MIDI_Transmit_FS(MidiTxBufferFS, 4) ) ? 1 : 0;
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

void setOutLed(bool on)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setInLed(bool on)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setOverflowLed(bool on)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
