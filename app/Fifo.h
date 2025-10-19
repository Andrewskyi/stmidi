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

#ifndef __FIFO_H_
#define __FIFO_H_

#include <stdint.h>
#include <FifoWriteElementFunc.h>
#include <type_traits>


template <typename T, typename WriteFunc=Fifo_writeFewElementsFunc<T> >
class Fifo {
public:
	volatile const bool& overflow;

	Fifo(T*const buf, const uint32_t length, WriteFunc writeFunc);
	virtual ~Fifo();

	bool write(const T* buf, uint32_t length);
	void tick();
protected:
	WriteFunc writeFunc;
private:
	T*const queue;
	const uint32_t FIFO_LENGTH;
	volatile uint32_t writePos;
	volatile uint32_t readPos;

	template <typename U = WriteFunc, typename std::enable_if<std::is_same<U, Fifo_writeFewElementsFunc<T> >::value, int>::type = 0>
	void consumeElements();

	template <typename U = WriteFunc, typename std::enable_if<std::is_same<U, Fifo_writeElementFunc<T> >::value, int>::type = 0>
	void consumeElements();

protected:
	volatile uint32_t fifoLen;
private:
	volatile bool _overflow = false;
};

template <typename T, typename WriteFunc>
Fifo<T, WriteFunc>::Fifo(T*const buf, const uint32_t length, WriteFunc writeFunc) :
        overflow(_overflow),
		writeFunc(writeFunc),
		queue(buf), FIFO_LENGTH(length),
		writePos(0), readPos(0), fifoLen(0) {

}

template <typename T, typename WriteFunc>
Fifo<T, WriteFunc>::~Fifo() {
}

template <typename T, typename WriteFunc>
bool Fifo<T, WriteFunc>::write(const T* buf, uint32_t length) {

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

		_overflow = false;

		return true;
	}

	_overflow = true;


	return false;
}


template <typename T, typename WriteFunc>
template <typename U, typename std::enable_if<std::is_same<U, Fifo_writeFewElementsFunc<T> >::value, int>::type>
void Fifo<T, WriteFunc>::consumeElements()
{
	if (fifoLen == 0) {
		return;
	}

	uint32_t headLen = fifoLen;
	uint32_t tailLen = 0;

	// correct if buffer is rotated
	if( (fifoLen > 0) && (readPos >= writePos) ) {
		headLen = FIFO_LENGTH - readPos;
		tailLen = fifoLen - headLen;
	}

	uint32_t consumed = 0;

	if(tailLen == 0) {
		consumed = writeFunc(&( queue[readPos]), headLen,  nullptr, 0);
	} else {
		consumed = writeFunc(&( queue[readPos]), headLen,  &( queue[0]), tailLen);
	}

	// fallback
	if(consumed > fifoLen) {
		consumed = fifoLen;
	}

	if (consumed > 0) {
		fifoLen -= consumed;
		readPos += consumed;

		if (readPos >= FIFO_LENGTH) {
			readPos -= FIFO_LENGTH;
		}
	}
}

template <typename T, typename WriteFunc>
template <typename U, typename std::enable_if<std::is_same<U, Fifo_writeElementFunc<T> >::value, int>::type>
void Fifo<T, WriteFunc>::consumeElements()
{
	if (fifoLen == 0) {
		return;
	}

	if ( writeFunc(queue[readPos]) ) {
		if (readPos < (FIFO_LENGTH - 1)) {
			readPos++;
		} else {
			readPos = 0;
		}

		fifoLen--;
	}
}

template <typename T, typename WriteFunc>
void Fifo<T, WriteFunc>::tick() {
	consumeElements();
}

#endif /* __FIFO_H_ */
