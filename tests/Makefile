# Makefile for gtest examples

GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include
GOOGLE_TEST_LIB_DIR = /usr/src/gtest

SOURCES = 
TESTS = FifoSingleElementFuncTests.cpp FifoFewElementsFuncTests.cpp

BUILD_DIR=obj
SRC_DIR=app
TARGET=tests

G++ = g++
G++_FLAGS = -g -std=gnu++14 -c -Wall -I $(GOOGLE_TEST_INCLUDE) -I ../$(SRC_DIR)
LD_FLAGS = -L $(GOOGLE_TEST_LIB_DIR) -l $(GOOGLE_TEST_LIB) -l pthread

OBJECTS = \
  $(BUILD_DIR)/GtestMain.o \
  $(TESTS:%.cpp=$(BUILD_DIR)/%.o) \
  $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJECTS:%.o=%.d)

all: $(TARGET)
	-./$(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

-include $(DEPS)

$(BUILD_DIR)/%.o : src/%.cpp
	mkdir -p $(BUILD_DIR)
	$(G++) $(G++_FLAGS) -MMD -o $@ $<

$(BUILD_DIR)/%.o : ../$(SRC_DIR)/%.cpp
	$(G++) $(G++_FLAGS) -MMD -o $@ $<

clean:
	-rm -f $(TARGET) $(OBJECTS) $(BUILD_DIR)/*
                    
.PHONY: all clean
