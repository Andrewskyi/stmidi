/*
 * TxFifo.h
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

#include <stdio.h>
#include <SystemOut.h>
#include <LowLevelFunctions.h>
#include <MidiSerialReceiver.h>
#include <MidiUsbSender.h>
#include <MidiSerialSender.h>
#include <MidiUsbReceiver.h>

SystemOut sysOut(&usart1send);

// serial to USB
MidiUsbSender usbMidiSender(&sendUsbMidi);
MidiSerialReceiver midiSerialReceiver(usbMidiSender);

// USB to serial
MidiSerialSender midiSerialSender(&usart2send);
MidiUsbReceiver midiUsbReceiver(midiSerialSender);

extern "C" int appMain(void)
{
	while(true)
	{
		// serial to USB
		usbMidiSender.tick();
		midiSerialReceiver.tick();

		// USB to serial
		midiSerialSender.tick();
		midiUsbReceiver.tick();

		// stdout
		sysOut.tick();
	}
}
