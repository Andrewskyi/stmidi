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

bool MidiSerialSender::sendMidi(const uint8_t* buf, uint32_t length)
{
	return fifo.write(buf, length);
}

bool MidiSerialSender::sendMidi(uint8_t b1, uint8_t b2, uint8_t b3)
{
	uint8_t buf[3] = {b1, b2, b3};

	return sendMidi(buf, 3);
}

bool MidiSerialSender::sendRealTimeMidi(uint8_t b)
{
	return sendMidi(&b, 1);
}

void MidiSerialSender::tick()
{
	fifo.tick();
}
