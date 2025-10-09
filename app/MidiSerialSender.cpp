/*
 * MidiSerialSender.cpp
 *
 *  Created on: Oct 6, 2025
 *      Author: apaluch
 */
#include <MidiSerialSender.h>

MidiSerialSender::MidiSerialSender(Fifo_writeElementFunc<uint8_t> writeFunc) :
  fifo(buf, MidiSerialSender_BUF_LEN, writeFunc)
{

}

bool MidiSerialSender::sendMidi(const MidiEvent& midiEvent)
{
	return fifo.write(midiEvent.buf, midiEvent.len);
}

bool MidiSerialSender::sendRealTimeMidi(uint8_t b)
{
	return fifo.write(&b, 1);
}

void MidiSerialSender::tick()
{
	fifo.tick();
}
