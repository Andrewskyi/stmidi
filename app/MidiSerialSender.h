/*
 * UsbMidiSender.h
 *
 *  Created on: Oct 5, 2025
 *      Author: apaluch
 */
#include <stdint.h>
#include <Fifo.h>
#include <MidiSender.h>

#ifndef MIDISERIALSENDER_H_
#define MIDISERIALSENDER_H_

#define MidiSerialSender_BUF_LEN 32

class MidiSerialSender : public MidiSender {
public:
	MidiSerialSender(Fifo_writeElementFunc<uint8_t> writeFunc);
	virtual ~MidiSerialSender(){}

	virtual bool sendMidi(const uint8_t* buf, uint32_t length);
	virtual bool sendMidi(uint8_t b1, uint8_t b2, uint8_t b3);
	virtual bool sendRealTimeMidi(uint8_t b);

	void tick();
private:
	uint8_t buf[MidiSerialSender_BUF_LEN];
    Fifo<uint8_t> fifo;
};

#endif /* MIDISERIALSENDER_H_ */
