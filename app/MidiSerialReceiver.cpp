/*
 * MidiReceiver.cpp
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

#include <MidiSerialReceiver.h>
#include <stdio.h>

MidiSerialReceiver::MidiSerialReceiver(MidiSender* midiThru) :
    fifo(buf, MidiSerialReceiver_BUF_LEN), midiThru(midiThru), runningStatusByte(0)
{
	midiEvent.len = 0;
}

MidiSerialReceiver::~MidiSerialReceiver() {
}

void MidiSerialReceiver::newUartByte(uint8_t b) {
	fifo.write(b);
}

void MidiSerialReceiver::nextEvent() {
	uint8_t b;

	if (fifo.read(b)) {
		// Channel Voice Message Header
		if (b >= 0x80 && b <= 0xEF) {
			runningStatusByte = b;
			midiEvent.len = 1;

			expectedBytesCount = expectedChannelVoiceMsgBytesCount();
		}
		// System Common Message Header
		else if (b >= 0xF0 && b <= 0xF7) {
			runningStatusByte = 0;
			midiEvent.buf[0] = b;

			switch (b) {
			case 0xF0: // sys exclusive
			case 0xF7: // end of system exclusive
				// ignore message
				expectedBytesCount = 0;
				midiEvent.len = 0;
				break;
			case 0xF1: // MIDI Time Code Quarter Frame
			case 0xF3: // Song Select
				expectedBytesCount = 2;
				midiEvent.len = 1;
				break;
			case 0xF2: // Song Position Pointer
				expectedBytesCount = 3;
				midiEvent.len = 1;
				break;
			case 0xF4: // undefined
			case 0xF5: // undefined
			case 0xF6: // tune request
				expectedBytesCount = 1;
				midiEvent.len = 1;
				break;
			}
		}
		// System Real-Time Message Header
		else if (b >= 0xF8) {
			midiThru->sendRealTimeMidi(b);
		}
        // additional bytes
		else if (b <= 127 && midiEvent.len < expectedBytesCount) {
			midiEvent.buf[midiEvent.len] = b;
			midiEvent.len++;
		}

		if (midiEvent.len > 0 && midiEvent.len == expectedBytesCount) {
			if (runningStatusByte > 0) {
				midiEvent.buf[0] = runningStatusByte;
			}

			//printf("%X %X %X\r\n", buf[0], buf[1], buf[2]);
			//printf("%X\r\n", buf[0]);

			if (midiThru) {
				midiThru->sendMidi(midiEvent);
			}

			if (runningStatusByte > 0) {
				midiEvent.len = 1;
				expectedBytesCount = expectedChannelVoiceMsgBytesCount();
			} else {
				midiEvent.len = 0;
				expectedBytesCount = 0;
			}

			//return true;
		}
	}

	//return false;
}

void MidiSerialReceiver::tick() {
	nextEvent();
}

uint32_t MidiSerialReceiver::expectedChannelVoiceMsgBytesCount() {
	switch (runningStatusByte & 0xF0) {
	case 0xC0:
	case 0xD0:
		return 2;
	default:
		return 3;
	}
}
