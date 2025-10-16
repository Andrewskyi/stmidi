/*
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

#ifndef LOWLEVELFUNCTIONS_H_
#define LOWLEVELFUNCTIONS_H_

#include <MidiUsbSender.h>
#include <stdint.h>

uint32_t usart1send(const uint8_t* b, uint32_t len);

bool usart1rec(uint8_t& b);

bool usart2rec(uint8_t& b);

uint32_t usart2send(const uint8_t* b, uint32_t len);

uint32_t sendUsbMidi(const UsbMidiEventPacket* packet, uint32_t len);

void setOutLed(bool on);

void setInLed(bool on);

void setOverflowLed(bool on);

#endif /* LOWLEVELFUNCTIONS_H_ */
