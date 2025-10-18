
#include <vector>
#include <gtest/gtest.h>
#include <Fifo.h>
#include <memory.h>
#include <algorithm>

#ifndef TEST_F
#define TEST_F(x, y) struct x##y : public x { void something(); }; x##y::something()
#endif



uint32_t FifoTest_witeManyFunc(const uint8_t* buf, uint32_t len)
{
  return len;
}

std::vector<uint8_t> singleWriteElements;
bool singleWriteRet = true;
bool FifoTest_witeSingleFunc(uint8_t element)
{
	singleWriteElements.push_back(element);
    return singleWriteRet;
}

class FifoWriteSingleTest : public ::testing::Test
{
protected:
	FifoWriteSingleTest() : uut(buf, 5, FifoTest_witeSingleFunc){}
	void SetUp(){}

	uint8_t buf[5];
	Fifo<uint8_t, Fifo_writeElementFunc<uint8_t> > uut;
};

TEST_F(FifoWriteSingleTest, writingElements)
{
	uint8_t tmp[] = {1, 2, 3, 4, 5};

	singleWriteElements.clear();
	singleWriteRet = true;
	memset(buf, 0, sizeof(buf));

	// write 5 elements
	EXPECT_TRUE( uut.write(&tmp[0], 1) );
	EXPECT_TRUE( uut.write(&tmp[1], 4) );

	// internal buf contains written bytes
	EXPECT_TRUE(std::equal(std::begin(buf), std::end(buf), std::begin(tmp)));

	// further writing should fail until elements are consumed...
	EXPECT_FALSE( uut.write(&tmp[0], 1) );
	EXPECT_FALSE( uut.write(&tmp[1], 4) );

	// consume element by element
	uut.tick();uut.tick();uut.tick();uut.tick();uut.tick();

	// function wrote elements
	std::vector<uint8_t> tmVect = {1, 2, 3, 4, 5};
    EXPECT_EQ(singleWriteElements, tmVect);

    /// now further writing is possible
    singleWriteElements.clear();
    singleWriteRet = false;
    EXPECT_TRUE( uut.write(&tmp[0], 1) );
    EXPECT_TRUE( uut.write(&tmp[1], 4) );
    // but writing func return false, so ticks have no side effect on fifo
    uut.tick();uut.tick();uut.tick();uut.tick();uut.tick();
    // fifo still try to write first element
    std::vector<uint8_t> same1 = {1, 1, 1, 1, 1};
    EXPECT_EQ(singleWriteElements, same1);
}



