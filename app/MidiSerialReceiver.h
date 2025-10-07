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

#ifndef MIDISERIALRECEIVER_H_
#define MIDISERIALRECEIVER_H_

#include <stdint.h>
#include <MidiSender.h>
#include <SafeFifo.h>

#define  MidiSerialReceiver_BUF_LEN 32

class MidiSerialReceiver {
public:
	MidiSerialReceiver(MidiSender* midiThru);
	virtual ~MidiSerialReceiver();

	void newUartByte(uint8_t b);
	bool nextEvent(uint8_t& b1, uint8_t& b2, uint8_t& b3);
	void tick();
private:
	uint8_t buf[MidiSerialReceiver_BUF_LEN];
	SafeFifo<uint8_t> fifo;
	MidiSender* midiThru;
	volatile uint8_t runningStatusByte;
	volatile uint32_t expectedBytesCount;
	uint8_t midiCommandBuf[3];
	volatile uint32_t bytesCount;
	volatile uint8_t systemRealTimeEvent;

	uint32_t expectedChannelVoiceMsgBytesCount();
	void trySendSystemRealtime();
};

#endif /* MIDISERIALRECEIVER_H_ */
