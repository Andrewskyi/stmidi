/*
 * TxFifo.h
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

#ifndef __FIFO_H_
#define __FIFO_H_

#include <stdint.h>
#include <FifoWriteElementFunc.h>

template <typename T, typename WriteFuncParamT=T>
class Fifo {
public:
	volatile const bool& overflow;

	Fifo(T*const buf, const uint32_t length, Fifo_writeElementFunc<WriteFuncParamT> writeFunc);
	virtual ~Fifo();

	bool write(const T* buf, uint32_t length);
	void tick();
protected:
	Fifo_writeElementFunc<WriteFuncParamT> writeFunc;
private:
	T*const queue;
	const uint32_t FIFO_LENGTH;
	volatile uint32_t writePos;
	volatile uint32_t readPos;
protected:
	volatile uint32_t fifoLen;
private:
	volatile bool _overflow = false;
};

template <typename T, typename WriteFuncParamT>
Fifo<T, WriteFuncParamT>::Fifo(T*const buf, const uint32_t length, Fifo_writeElementFunc<WriteFuncParamT> writeFuncParam) :
        overflow(_overflow),
		writeFunc(writeFuncParam),
		queue(buf), FIFO_LENGTH(length),
		writePos(0), readPos(0), fifoLen(0) {

}

template <typename T, typename WriteFuncParamT>
Fifo<T, WriteFuncParamT>::~Fifo() {
}

template <typename T, typename WriteFuncParamT>
bool Fifo<T, WriteFuncParamT>::write(const T* buf, uint32_t length) {

	if ((fifoLen + length) <= FIFO_LENGTH) {
		for (uint32_t i = 0; i < length; i++) {
			queue[writePos] = buf[i];

			if (writePos < (FIFO_LENGTH - 1)) {
				writePos++;
			} else {
				writePos = 0;
			}
		}

		fifoLen += length;
		tick();

		_overflow = false;

		return true;
	}

	_overflow = true;


	return false;
}

template <typename T, typename WriteFuncParamT>
void Fifo<T, WriteFuncParamT>::tick() {
	if (fifoLen == 0) {
		return;
	}

	if (writeFunc(queue[readPos])) {
		if (readPos < (FIFO_LENGTH - 1)) {
			readPos++;
		} else {
			readPos = 0;
		}

		fifoLen--;
	}
}

#endif /* __FIFO_H_ */
