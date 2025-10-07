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
		fifo(buf, MidiSerialReceiver_BUF_LEN), midiThru(midiThru), runningStatusByte(0), bytesCount(
				0), systemRealTimeEvent(0) {
}

MidiSerialReceiver::~MidiSerialReceiver() {
}

void MidiSerialReceiver::newUartByte(uint8_t b) {
	fifo.write(b);
}

bool MidiSerialReceiver::nextEvent(uint8_t& b1, uint8_t& b2, uint8_t& b3) {
	uint8_t b;

	if (fifo.read(b)) {
		// Channel Voice Message
		if (b >= 0x80 && b <= 0xEF) {
			runningStatusByte = b;
			bytesCount = 1;

			expectedBytesCount = expectedChannelVoiceMsgBytesCount();
		}
		// Channel Voice Message

		// System Common Messages
		else if (b >= 0xF0 && b <= 0xF7) {
			runningStatusByte = 0;
			midiCommandBuf[0] = b;

			switch (b) {
			case 0xF0:
			case 0xF7:
				// ignore message
				expectedBytesCount = 0;
				bytesCount = 0;
				break;
			case 0xF1:
			case 0xF3:
				expectedBytesCount = 2;
				bytesCount = 1;
				break;
			case 0xF2:
				expectedBytesCount = 3;
				bytesCount = 1;
				break;
			case 0xF4:
			case 0xF5:
			case 0xF6:
				expectedBytesCount = 1;
				bytesCount = 1;
				break;
			}
		}
		// System Common Messages

		// System Real-Time Messages
		else if (b >= 0xF8) {
			systemRealTimeEvent = b;
			trySendSystemRealtime();
		}
		// System Real-Time Messages

		else if (b <= 127 && bytesCount < expectedBytesCount) {
			midiCommandBuf[bytesCount] = b;
			bytesCount++;
		}

		bool eventForLooper = false;

		if (bytesCount > 0 && bytesCount == expectedBytesCount) {
			if (runningStatusByte > 0) {
				midiCommandBuf[0] = runningStatusByte;
			}

			//printf("%X %X %X\r\n", buf[0], buf[1], buf[2]);
			//printf("%X\r\n", buf[0]);

			if (midiThru) {
				midiThru->sendMidi(midiCommandBuf, bytesCount);
			}

			if (bytesCount == 3 &&
				(
				    (runningStatusByte & 0xF0) == 0x80 ||
					(runningStatusByte & 0xF0) == 0x90 ||
					(runningStatusByte & 0xF0) == 0xB0
				)
			)
			{
				b1 = midiCommandBuf[0];
				b2 = midiCommandBuf[1];
				b3 = midiCommandBuf[2];

				eventForLooper = true;
			}

			if (runningStatusByte > 0) {
				bytesCount = 1;
				expectedBytesCount = expectedChannelVoiceMsgBytesCount();
			} else {
				bytesCount = 0;
				expectedBytesCount = 0;
			}
		}

		return eventForLooper;
	}

	return false;
}

void MidiSerialReceiver::tick() {
	trySendSystemRealtime();
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

void MidiSerialReceiver::trySendSystemRealtime() {
	if (systemRealTimeEvent) {
		if (midiThru) {
			if (midiThru->sendRealTimeMidi(systemRealTimeEvent)) {
				systemRealTimeEvent = 0;
			}
		} else {
			systemRealTimeEvent = 0;
		}
	}
}
