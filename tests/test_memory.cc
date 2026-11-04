#include <gtest/gtest.h>
#include <string>

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

TEST(TestMemory, TestWrite32){
    // Happy path test
    Memory target = Memory(256);
    uint32_t address = 0xF7;
    target.write32(address, 0xFFFFFFFF);
    uint32_t result = static_cast<uint32_t>(target.data()[address]) << 24 | 
                        static_cast<uint32_t>(target.data()[address + 1]) << 16 | 
                        static_cast<uint32_t>(target.data()[address + 2]) << 8 | 
                        static_cast<uint32_t>(target.data()[address + 3]);
    ASSERT_EQ(result, 0xFFFFFFFF);

    // Out of index test
    address = 0xFF;
    ASSERT_THROW(target.write32(address, 0xFFFFFFFF), std::out_of_range);
}

TEST(TestMemory, TestRead8){
    // Happy path test
    Memory target = Memory(256);
    uint32_t address = 0xFF;
    target.write8(address, 0xAB);
    ASSERT_EQ(target.read8(address), 0xAB);
    ASSERT_EQ(target.read8Int(address), 0xAB);

    // Out of index test
    address = 0x100;
    ASSERT_THROW(target.read8(address), std::out_of_range);
    ASSERT_THROW(target.read8Int(address), std::out_of_range);
}

TEST(TestMemory, TestRead32){
    // Happy path test
    Memory target = Memory(256);
    uint32_t address = 0xF7;
    target.write32(address, 0x12345678);
    ASSERT_EQ(target.read32(address), 0x12345678);
    ASSERT_EQ(target.read32Int(address), 0x12345678);

    // Out of index test
    address = 0xFF;
    ASSERT_THROW(target.read32(address), std::out_of_range);
    ASSERT_THROW(target.read32Int(address), std::out_of_range);

    // Print test
    address = 0xF7;
    testing::internal::CaptureStdout();
    target.print32Hex(address);

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "12 34 56 78\n");
}