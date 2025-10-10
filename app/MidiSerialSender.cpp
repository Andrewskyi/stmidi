/*
 * MidiSerialSender.cpp
 *
 *  Created on: Oct 6, 2025
 *      Author: apaluch
 */
#include <MidiSerialSender.h>


MidiSerialSender::MidiSerialSender(Fifo_writeElementFunc<uint8_t> writeFunc) :
  fifo(buf, MidiSerialSender_BUF_LEN, writeFunc), runningStatus(0)
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
