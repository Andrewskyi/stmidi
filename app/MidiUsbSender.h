/*
 * 
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

#include <stdint.h>
#include <stdbool.h>
#include <Fifo.h>
#include <MidiSender.h>
#include <UsbMidiEventPacket.h>

#ifndef USBMIDISENDER_H_
#define USBMIDISENDER_H_

#define MidiUsbSender_BUF_LEN 1024

class MidiUsbSender : public MidiSender {
public:
	using WriteFunctionParamT = UsbMidiEventPacket;
	MidiUsbSender(Fifo_writeFewElementsFunc<WriteFunctionParamT> writeFunc);
	virtual ~MidiUsbSender();

	virtual bool sendMidi(const MidiEvent& midiEvent);
	virtual bool sendRealTimeMidi(uint8_t b);

	void tick();
private:
	UsbMidiEventPacket buf[MidiUsbSender_BUF_LEN];
    Fifo<UsbMidiEventPacket, WriteFunctionParamT> fifo;

	UsbMidiEventPacket midiToUSB(uint8_t virtualCable, const uint8_t *midiMsg, uint8_t length);
public:
	volatile const bool& overflow;
};

#endif /* USBMIDISENDER_H_ */
