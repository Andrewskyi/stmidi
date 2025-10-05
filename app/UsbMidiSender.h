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

#ifndef USBMIDISENDER_H_
#define USBMIDISENDER_H_

#define UsbMidiSender_BUF_LEN 32

/* USB-MIDI 4-byte event packet */
struct USB_MIDI_EventPacket_t {
	uint8_t bytes[4];
};

class UsbMidiSender : public MidiSender {
public:
	UsbMidiSender(Fifo_writeByteFunc<USB_MIDI_EventPacket_t> writeFunc);
	virtual ~UsbMidiSender();

	virtual bool sendMidi(const uint8_t* buf, uint32_t length);
	virtual bool sendMidi(uint8_t b1, uint8_t b2, uint8_t b3);
	virtual bool sendRealTimeMidi(uint8_t b);

	void tick();
private:
	USB_MIDI_EventPacket_t buf[UsbMidiSender_BUF_LEN];
    Fifo<USB_MIDI_EventPacket_t> fifo;

	USB_MIDI_EventPacket_t midiToUSB(uint8_t virtualCable, const uint8_t *midiMsg, uint8_t length);
};

#endif /* USBMIDISENDER_H_ */
