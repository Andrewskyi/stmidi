/*
 * LowLevelFunctions.h
 *
 *  Created on: Oct 5, 2025
 *      Author: apaluch
 */

#ifndef LOWLEVELFUNCTIONS_H_
#define LOWLEVELFUNCTIONS_H_

#include <MidiUsbSender.h>
#include <stdint.h>

bool usart1send(char b);

bool usart1rec(char* b);

bool usart2rec(uint8_t& b);

bool sendUsbMidi(UsbMidiEventPacket package);


#endif /* LOWLEVELFUNCTIONS_H_ */
