#include <stdio.h>
#include <SystemOut.h>
#include <LowLevelFunctions.h>
#include <MidiSerialReceiver.h>
#include <MidiUsbSender.h>
#include <MidiSerialSender.h>
#include <MidiUsbReceiver.h>

SystemOut sysOut(&usart1send);

// serial to USB
MidiUsbSender usbMidiSender(&sendUsbMidi);
MidiSerialReceiver midiSerialReceiver(usbMidiSender);

// USB to serial
MidiSerialSender midiSerialSender(&usart2send);
MidiUsbReceiver midiUsbReceiver(midiSerialSender);

extern "C" int appMain(void)
{
	//printf("Hello world\n\r");

	while(true)
	{
		// serial to USB
		usbMidiSender.tick();
		midiSerialReceiver.tick();

		// USB to serial
		midiSerialSender.tick();
		midiUsbReceiver.tick();

		// stdout
		sysOut.tick();
	}
}
