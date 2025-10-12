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

#include <stdint.h>
#include <Fifo.h>
#include <MidiSender.h>

#ifndef MIDISERIALSENDER_H_
#define MIDISERIALSENDER_H_

#define MidiSerialSender_BUF_LEN 2048

class MidiSerialSender : public MidiSender {
public:
	MidiSerialSender(Fifo_writeElementFunc<uint8_t> writeFunc);
	virtual ~MidiSerialSender(){}

	virtual bool sendMidi(const MidiEvent& midiEvent);
	virtual bool sendRealTimeMidi(uint8_t b);

	void tick();
private:
	uint8_t buf[MidiSerialSender_BUF_LEN];
    Fifo<uint8_t> fifo;
    uint8_t runningStatus;
};

#endif /* MIDISERIALSENDER_H_ */
