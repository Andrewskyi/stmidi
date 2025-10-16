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

#include <MidiUsbSender.h>

MidiUsbSender::MidiUsbSender(Fifo_writeFewElementsFunc<WriteFunctionParamT> writeFunc) :
	fifo(buf, MidiUsbSender_BUF_LEN, writeFunc),
	overflow(fifo.overflow)
{

}

MidiUsbSender::~MidiUsbSender()
{

}

bool MidiUsbSender::sendMidi(const MidiEvent& midiEvent)
{
	UsbMidiEventPacket package = midiToUSB(0, midiEvent.buf, midiEvent.len);

	return fifo.write(&package, 1);
}

bool MidiUsbSender::sendRealTimeMidi(uint8_t b)
{
	UsbMidiEventPacket package = midiToUSB(0, &b, 1);

	return fifo.write(&package, 1);
}

void MidiUsbSender::tick()
{
	fifo.tick();
}

/* Generate USB-MIDI packet from 1–3 byte MIDI message */
UsbMidiEventPacket MidiUsbSender::midiToUSB(uint8_t virtualCable, const uint8_t *midiMsg, uint8_t length)
{
    UsbMidiEventPacket packet = {0};

    uint8_t status = midiMsg[0];
    uint8_t cin = 0;

    // Determine CIN (Code Index Number)
    if (status >= 0x80 && status <= 0xEF) {
        // Channel Voice messages (Note Off/On/Poly Pressure/Control Change/Program/Channel Pressure/Pitch Bend)
        switch (status & 0xF0) {
            case 0x80: cin = 0x8; break; // Note Off
            case 0x90: cin = 0x9; break; // Note On
            case 0xA0: cin = 0xA; break; // Poly Key Pressure
            case 0xB0: cin = 0xB; break; // Control Change
            case 0xC0: cin = 0xC; break; // Program Change
            case 0xD0: cin = 0xD; break; // Channel Pressure
            case 0xE0: cin = 0xE; break; // Pitch Bend
        }
    } else {
        // System Common & System Real-Time messages
        switch (status) {
            case 0xF0: cin = 0x4; break; // SysEx start
            case 0xF7:
                if (length == 1) cin = 0x5; // SysEx ends with 1 byte
                else if (length == 2) cin = 0x6; // ends with 2 bytes
                else if (length == 3) cin = 0x7; // ends with 3 bytes
                break;
            case 0xF1: cin = 0x2; break; // MTC Quarter Frame
            case 0xF2: cin = 0x3; break; // Song Position Pointer
            case 0xF3: cin = 0x2; break; // Song Select
            default: cin = 0xF; break;      // Single-byte real-time messages
        }
    }

    // Byte 0 = Cable Number (upper 4 bits) + CIN (lower 4 bits)
    packet.bytes[0] = (virtualCable << 4) | (cin & 0x0F);

    // Copy MIDI bytes into packet (bytes 1–3)
    for (uint8_t i = 0; i < 3; i++) {
        if (i < length) {
            packet.bytes[i + 1] = midiMsg[i];
        } else {
            packet.bytes[i + 1] = 0; // pad with 0 if shorter
        }
    }

    return packet;
}
