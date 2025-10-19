
#include <vector>
#include <gtest/gtest.h>
#include <Fifo.h>
#include <memory.h>
#include <algorithm>

// just for IDE indexer
#ifndef TEST_F
#define TEST_F(x, y) struct x##y : public x { void something(); }; x##y::something()
#endif

std::vector<uint8_t> fewWriteElements;
uint32_t fewWriteRet = 1;

uint32_t FifoTest_witeFewFunc(const uint8_t* part1, uint32_t len1, const uint8_t* part2, uint32_t len2)
{
	if(len1 > fewWriteRet) {
		len1 = fewWriteRet;
	} else if((len1 + len2) > fewWriteRet) {
		len2 -= (fewWriteRet - len1);
	}

	if(len1 > 0) {
		fewWriteElements.insert(fewWriteElements.end(), part1, part1 + len1);
	}

	if(len2 > 0) {
		fewWriteElements.insert(fewWriteElements.end(), part2, part2 + len2);
	}
    return fewWriteRet;
}

class FifoWriteFewTest : public ::testing::Test
{
protected:
	FifoWriteFewTest() : uut(buf, 5, &FifoTest_witeFewFunc){}
	void SetUp()
	{
		fewWriteElements.clear();
		fewWriteRet = 0;
		memset(buf, 0, sizeof(buf));
	}

	uint8_t buf[5];
	Fifo<uint8_t, Fifo_writeFewElementsFunc<uint8_t> > uut;
};

TEST_F(FifoWriteFewTest, writingElements)
{
	uint8_t tmp[] = {1, 2, 3, 4, 5};

	// write 5 elements
	EXPECT_TRUE( uut.write(&tmp[0], 2) );
	EXPECT_TRUE( uut.write(&tmp[2], 3) );

	// internal buf contains written bytes
	EXPECT_TRUE(std::equal(std::begin(buf), std::end(buf), std::begin(tmp)));

	// further writing should fail until elements are consumed...
	EXPECT_FALSE( uut.write(&tmp[0], 1) );
	EXPECT_FALSE( uut.write(&tmp[1], 4) );

	// consume 3 elements
	fewWriteRet = 3;
	uut.tick();

    // function should received first 2 elements
	EXPECT_EQ(fewWriteElements, std::vector<uint8_t>({1, 2, 3}));

	// write two elements
	tmp[0] = 6; tmp[1] = 7;
	EXPECT_TRUE( uut.write(&tmp[0], 2) );

	// fifo should contain 4 elements, read them all
	fewWriteElements.clear();
	fewWriteRet = 4;
    uut.tick();

    EXPECT_EQ(fewWriteElements, std::vector<uint8_t>({4, 5, 6, 7}));
}

TEST_F(FifoWriteFewTest, functionReturnTooManyConsumed)
{
	uint8_t tmp[] = {1, 2, 3, 4, 5};

	// write 5 elements
	EXPECT_TRUE( uut.write(&tmp[0], 2) );
	EXPECT_TRUE( uut.write(&tmp[2], 3) );

	// internal buf contains written bytes
	EXPECT_TRUE(std::equal(std::begin(buf), std::end(buf), std::begin(tmp)));

	// report consume 23 elements
	fewWriteRet = 23;
	uut.tick();

    // but fifo should be able to deal with wrong return from write func and should simply assume "max number of elements are consumed"
	EXPECT_EQ(fewWriteElements, std::vector<uint8_t>({1, 2, 3, 4, 5}));
}

