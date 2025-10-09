/*
 * MidiEvent.h
 *
 *  Created on: Oct 9, 2025
 *      Author: apaluch
 */

#ifndef MIDIEVENT_H_
#define MIDIEVENT_H_

#include <stdint.h>

struct MidiEvent
{
	uint8_t buf[3];
	uint8_t len;
};

#endif /* MIDIEVENT_H_ */
