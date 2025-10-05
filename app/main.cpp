#include <stdio.h>
#include <SystemOut.h>
#include <LowLevelFunctions.h>
#include <MidiSerialReceiver.h>
#include <MidiUsbSender.h>

SystemOut sysOut(usart1send);
MidiUsbSender usbMidiSender(&sendUsbMidi);
MidiSerialReceiver serialMidiReceiver(&usart2rec, &usbMidiSender);

extern "C" int appMain(void)
{
	//printf("Hello world\n\r");

	while(true)
	{
		uint8_t b1, b2, b3;

		if(serialMidiReceiver.nextEvent(b1, b2, b3)){
			//printf("midi\n");
			// place to trigger LED
		}

		serialMidiReceiver.tick();
		usbMidiSender.tick();
		sysOut.tick();
	}
}
