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
