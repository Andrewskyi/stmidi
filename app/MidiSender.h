/*
 * MidiSender.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 *
 *      MIT License

Copyright (c) 2020 apaluch

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

#ifndef MIDISENDER_H_
#define MIDISENDER_H_
#include <stdint.h>

class MidiSender
{
public:
	virtual ~MidiSender(){}

	virtual bool sendMidi(const uint8_t* buf, uint32_t length);
	virtual bool sendMidi(uint8_t b1, uint8_t b2, uint8_t b3);
	virtual bool sendRealTimeMidi(uint8_t b);
};

#endif /* MIDISENDER_H_ */
