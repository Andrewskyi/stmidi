/*
 * MidiUsbReceiver.cpp
 *
 *  Created on: Oct 6, 2025
 *      Author: apaluch
 */

#include <MidiUsbReceiver.h>

MidiUsbReceiver::MidiUsbReceiver(MidiSender& midiThru) :
    inputFifo(buf, MidiUsbReceiver_BUF_LEN), midiThru(midiThru)
{

}

MidiUsbReceiver::~MidiUsbReceiver()
{

}

bool MidiUsbReceiver::newUsbEvent(const UsbMidiEventPacket& packet)
{
	return inputFifo.write(packet);
}

void MidiUsbReceiver::tick()
{
	if(inputFifo.peek(packet)) {
		midiEvent.event.len = serialMidiLength(0x0F & packet.bytes[0] /*cin*/);

		if( midiThru.sendMidi(midiEvent.event) ) {
			inputFifo.skeep();
		}
	}
}

uint8_t MidiUsbReceiver::serialMidiLength(uint8_t cin)
{
	switch (cin) {
		case 0x2: return 2;
		case 0x3: return 3;
		case 0x4: return 3;
		case 0x5: return 1;
		case 0x6: return 3;
		case 0x7: return 1;
		case 0x8: return 3;
		case 0x9: return 3;
		case 0xA: return 3;
		case 0xB: return 3;
		case 0xC: return 2;
		case 0xD: return 2;
		case 0xE: return 3;
		case 0xF: return 1;
		default:  return 0; // reserved or undefined
	}
}


