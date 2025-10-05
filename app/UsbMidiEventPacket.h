/*
 * UsbMidiEventPacket.h
 *
 *  Created on: Oct 5, 2025
 *      Author: apaluch
 */

#ifndef USBMIDIEVENTPACKET_H_
#define USBMIDIEVENTPACKET_H_

/* USB-MIDI 4-byte event packet */
struct UsbMidiEventPacket {
	uint8_t bytes[4];
};

#endif /* USBMIDIEVENTPACKET_H_ */
