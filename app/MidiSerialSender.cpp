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

#include <MidiSerialSender.h>


MidiSerialSender::MidiSerialSender(Fifo_writeFewElementsFunc<uint8_t> writeFunc) :
  fifo(buf, MidiSerialSender_BUF_LEN, writeFunc), runningStatus(0),
  overflow(fifo.overflow)
{

}

bool MidiSerialSender::sendMidi(const MidiEvent& midiEvent)
{
	// empty event not allowed
	if(midiEvent.len < 1) return false;

	const uint8_t& header = midiEvent.buf[0];
	const auto& bufPtr = (header==runningStatus) ? (&midiEvent.buf[1]) : &midiEvent.buf[0];
	const auto& bufLen = (header==runningStatus) ? (midiEvent.len - 1) : midiEvent.len;

	bool result = fifo.write(bufPtr, bufLen);

	if ( result && (header >= 0x80 && header <= 0xEF) ) {
		runningStatus = header;
	} if ( !result || (header >= 0xF0 && header <= 0xF7) ) {
		runningStatus = 0;
	}

	return result;
}

bool MidiSerialSender::sendRealTimeMidi(uint8_t b)
{
	return fifo.write(&b, 1);
}

void MidiSerialSender::tick()
{
	fifo.tick();
}
