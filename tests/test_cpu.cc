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