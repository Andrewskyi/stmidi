/*
 * MidiReceiver.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 *
 *      MIT License

Copyright (c) 2020 apaluch

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

#ifndef MIDIUSBRECEIVER_H_
#define MIDIUSBRECEIVER_H_

#include <stdint.h>
#include <UsbMidiEventPacket.h>
#include <MidiEvent.h>
#include <MidiSender.h>
#include <SafeFifo.h>


#define MidiUsbReceiver_BUF_LEN 1200

class MidiUsbReceiver {
public:
	MidiUsbReceiver(MidiSender& midiThru);
	virtual ~MidiUsbReceiver();

	// safe function may be invoked in interrupt context
	bool newUsbEvent(const UsbMidiEventPacket& packet);

	// unsafe function, called in main loop
	void tick();
private:
	SafeFifo<UsbMidiEventPacket> inputFifo;
	MidiSender& midiThru;
	UsbMidiEventPacket buf[MidiUsbReceiver_BUF_LEN];
	struct MidiEventWithUsbHeader {
		uint8_t usbHeader;
		MidiEvent event;
	};
	union {
	  UsbMidiEventPacket packet;
	  MidiEventWithUsbHeader midiEvent;
	};
	uint8_t serialMidiLength(uint8_t cin);
};

#endif /* MIDIUSBRECEIVER_H_ */
