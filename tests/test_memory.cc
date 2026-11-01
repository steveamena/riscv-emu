#include <gtest/gtest.h>

#include "../src/memory.h"

TEST(TestMemory, TestWrite8){
    // Happy path test
    Memory target = Memory(256);
    uint32_t address = 0xFF;
    target.write8(address, 0xAB);
    ASSERT_EQ(target.data()[address], 0xAB);

    // Out of index test
    address = 0x100;
    ASSERT_THROW(target.write8(address, 0xAB), std::out_of_range);
}

TEST(TestMemory, TestRead8){
    // Happy path test
    Memory target = Memory(256);
    uint32_t address = 0xFF;
    target.write8(address, 0xAB);
    ASSERT_EQ(target.read8(address), 0xAB);

    // Out of index test
    address = 0x100;
    ASSERT_THROW(target.read8(address), std::out_of_range);
}