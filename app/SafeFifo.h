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

#ifndef __SAFE_FIFO__
#define __SAFE_FIFO__

#include <stdint.h>

template<typename T>
class SafeFifo {
public:
	volatile const bool& overflow;

	SafeFifo(T*const buf, const uint32_t length) : overflow(_overflow), buf(buf), FIFO_LENGTH(length) {}

    bool write(const T& element) {
        uint32_t next = incIdx(writePos);

        if (next == readPos) {
        	_overflow = true;
        	return false;
        } else {
        	_overflow = false;
        }

        buf[writePos] = element;
        writePos = next;

        return true;
    }

    bool read(T& element) {
        if (writePos == readPos) {
        	return false;
        }

        element = buf[readPos];
        readPos = incIdx(readPos);
        _overflow = false;

        return true;
    }

    bool peek(T& element) {
		if (writePos == readPos) {
			return false;
		}

		element = buf[readPos];

		return true;
	}

    bool skeep() {
		if (writePos == readPos) {
			return false;
		}

		readPos = incIdx(readPos);
		_overflow = false;

		return true;
	}

    bool dataAvailable() const {
        return writePos != readPos;
    }

private:
    volatile uint32_t writePos = 0;
    volatile uint32_t readPos = 0;
    T*const buf;
    const uint32_t FIFO_LENGTH;
    volatile bool _overflow = false;

    uint32_t incIdx(uint32_t idx)
    {
    	idx++;

    	if(idx == FIFO_LENGTH) {
    		idx = 0;
    	}

    	return idx;
    }
};

#endif // __SAFE_FIFO__
