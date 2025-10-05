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
#include <MidiSender.h>
#include <SafeFifo.h>

/*

 bool isValidUsbMidiheader = ( (data[i] & 0x0F) <= 0x0F && (data[i+1] & 0x80) );

uint8_t usb_midi_valid_length(uint8_t cin)
{
    switch (cin) {
        case 0x2: return 2;
        case 0x3: return 3;
        case 0x4: return 3;
        case 0x5: return 1;
        case 0x6: return 3;
        case 0x7: return 1;
        case 0x8: return 3;
        case 0x9: return 3;
        case 0xA: return 3;
        case 0xB: return 3;
        case 0xC: return 2;
        case 0xD: return 2;
        case 0xE: return 3;
        case 0xF: return 1;
        default:  return 0; // reserved or undefined
    }
}

 */

#define MidiUsbReceiver_BUF_LEN 24

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

	uint8_t serialMidiLength(uint8_t cin);
};

#endif /* MIDIUSBRECEIVER_H_ */
