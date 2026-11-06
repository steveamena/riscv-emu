#include <gtest/gtest.h>

#include "../src/memory.h"
#include "../src/cpu.h"

TEST(CpuTest, FetchesInstructionAtProgramCounter) {
    Memory memory(1024);
    CPU target(memory);

    memory.write32(0xFC, 0x00100093); // ADDI x1, x0, 1
    target.setPC(0xFC);

    target.step();

    EXPECT_EQ(target.readRegister(1), 1u);
}

TEST(CpuTest, ExecutesImmediateInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t expected;
    };

    const TestCase test_cases[] = {
        {0xFFF08113, 10, 9},          // ADDI x2, x1, -1
        {0x0030A113, 0xFFFFFFFF, 1}, // SLTI x2, x1, 1
        {0x0070B113, 5, 1},           // SLTIU x2, x1, 7
        {0x0050C113, 0x0F, 0x0A},     // XORI x2, x1, 5
        {0x0030E113, 0x10, 0x13},     // ORI x2, x1, 3
        {0x00F0F113, 0xFF, 0x0F},     // ANDI x2, x1, 15
        {0x00209113, 3, 12},          // SLLI x2, x1, 2
        {0x0020D113, 0x20, 8},        // SRLI x2, x1, 2
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0xFC;

    for (const TestCase& test_case : test_cases) {
        target.writeRegister(1, test_case.rs1_value);
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();

        EXPECT_EQ(target.readRegister(2), test_case.expected);
    }
}