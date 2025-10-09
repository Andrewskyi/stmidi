/*
 * UsbMidiSender.h
 *
 *  Created on: Oct 5, 2025
 *      Author: apaluch
 */
#include <stdint.h>
#include <stdbool.h>
#include <Fifo.h>
#include <MidiSender.h>
#include <UsbMidiEventPacket.h>

#ifndef USBMIDISENDER_H_
#define USBMIDISENDER_H_

#define MidiUsbSender_BUF_LEN 1024

class MidiUsbSender : public MidiSender {
public:
	MidiUsbSender(Fifo_writeElementFunc<UsbMidiEventPacket> writeFunc);
	virtual ~MidiUsbSender();

	virtual bool sendMidi(const MidiEvent& midiEvent);
	virtual bool sendRealTimeMidi(uint8_t b);

	void tick();
private:
	UsbMidiEventPacket buf[MidiUsbSender_BUF_LEN];
    Fifo<UsbMidiEventPacket> fifo;

	UsbMidiEventPacket midiToUSB(uint8_t virtualCable, const uint8_t *midiMsg, uint8_t length);
};

#endif /* USBMIDISENDER_H_ */
