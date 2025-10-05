/*
 * FifoWriteElementFunc.h
 *
 *  Created on: Oct 5, 2025
 *      Author: apaluch
 */

#ifndef FIFOWRITEELEMENTFUNC_H_
#define FIFOWRITEELEMENTFUNC_H_

template <typename T>
using Fifo_writeElementFunc = bool(*)(T element);

#endif /* FIFOWRITEELEMENTFUNC_H_ */
