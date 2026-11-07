#include <gtest/gtest.h>

#include "../src/memory.h"
#include "../src/cpu.h"

int32_t sign_extend(uint32_t value, uint32_t size) {
    if (size == 0 || size >= 32) {
        throw std::invalid_argument("Invalid sign-extension width");
    }

    const uint32_t mask = (uint32_t{1} << size) - 1;
    const uint32_t sign_bit = uint32_t{1} << (size - 1);

    value &= mask;
    if (value & sign_bit) {
        value |= ~mask;
    }
    return static_cast<int32_t>(value);
}

enum class TestOpcode : uint8_t {
    ALU_Imm = 0x13,
    Load = 0x03,
    Store = 0x23,
    Branch = 0x63,
    JAL = 0x6F,
    JALR = 0x67,
    LUI = 0x37,
    AUIPC = 0x17,
};

int32_t compute_inmediate(uint32_t instruction){
    switch(static_cast<TestOpcode>(instruction & 0x7F)){
        case TestOpcode::ALU_Imm:
        case TestOpcode::Load:
        case TestOpcode::JALR:{
            return sign_extend((instruction >> 20), 12);
        }
        case TestOpcode::Store:{
            int32_t imm_hi = (instruction >> 25) & 0x7F; // Bits [11:5]
            int32_t imm_lo = (instruction >> 7) & 0x1F;  // Bits [4:0]
            int32_t imm = (imm_hi << 5) | imm_lo;
            return sign_extend(imm, 12);
        }
        case TestOpcode::Branch:{
            const int32_t immediate = (((instruction >> 31) & 0x1) << 12) |
                                       (((instruction >> 7) & 0x1) << 11) |
                                       (((instruction >> 25) & 0x3F) << 5) |
                                       (((instruction >> 8) & 0xF) << 1);
            return sign_extend(immediate, 13);
        }
        case TestOpcode::JAL:{
            const uint32_t bit20    = (instruction >> 31) & 0x1;
            const uint32_t bit19_12 = (instruction >> 12) & 0xFF;
            const uint32_t bit11    = (instruction >> 20) & 0x1;
            const uint32_t bit10_1  = (instruction >> 21) & 0x3FF;
 
            const uint32_t imm = (bit20 << 20) | (bit19_12 << 12) | (bit11 << 11) | (bit10_1 << 1);
            return sign_extend(imm, 21);
        }
        default:
            return 0;
    }
}
    

TEST(TestCPU, FetchesInstructionAtProgramCounter) {
    Memory memory(1024);
    CPU target(memory);

    memory.write32(0xFC, 0x00100093); // ADDI x1, x0, 1
    target.setPC(0xFC);

    target.step();

    EXPECT_EQ(target.readRegister(1), 1u);
}

TEST(TestCpu, RInstructions){
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t rs2_value;
        uint32_t expected;
    };

    const TestCase test_cases[] = {
        {0x00308133, 7, 5, 12},                    // ADD x2, x1, x3: x2 = x1 + x3
        {0x40308133, 7, 5, 2},                     // SUB x2, x1, x3: x2 = x1 - x3
        {0x00309133, 3, 2, 12},                    // SLL x2, x1, x3: x2 = x1 << x3
        {0x0030A133, 0xFFFFFFFF, 1, 1},            // SLT x2, x1, x3: x2 = signed(x1) < signed(x3)
        {0x0030B133, 1, 0xFFFFFFFF, 1},            // SLTU x2, x1, x3: x2 = unsigned(x1) < unsigned(x3)
        {0x0030C133, 0x0F, 0x33, 0x3C},            // XOR x2, x1, x3: x2 = x1 ^ x3
        {0x0030D133, 0x20, 2, 8},                  // SRL x2, x1, x3: x2 = x1 >> x3
        {0x4030D133, 0xFFFFFFE0, 2, 0xFFFFFFF8},   // SRA x2, x1, x3: x2 = signed(x1) >> x3
        {0x0030E133, 0x0F, 0x33, 0x3F},            // OR x2, x1, x3: x2 = x1 | x3
        {0x0030F133, 0x0F, 0x33, 0x03},            // AND x2, x1, x3: x2 = x1 & x3
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0xFC;

    for (const TestCase& test_case : test_cases) {
        target.writeRegister(1, test_case.rs1_value);
        target.writeRegister(3, test_case.rs2_value);
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();

        EXPECT_EQ(target.readRegister(2), test_case.expected);
    }
}

TEST(TestCpu, ImmediateInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t expected;
    };

    const TestCase test_cases[] = {
        {0x00008103, 0x100, 0xFFFFFFFF}, // LB  x2, 0(x1)
        {0x00009103, 0x100, 0xFFFFFFFF}, // LH  x2, 0(x1)
        {0x0000A103, 0x100, 0xFFFFFFFF}, // LW  x2, 0(x1)
        {0x0000C103, 0x100, 0x000000FF}, // LBU x2, 0(x1)
        {0x0000D103, 0x100, 0x0000FFFF}, // LHU x2, 0(x1)
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

TEST(TestCpu, LoadInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t expected;
    };

    const TestCase test_cases[] = {
        {0x00308103, 0x100, 0xFFFFFF80}, // LB  x2, 3(x1)
        {0x00209103, 0x100, 0xFFFF80FF}, // LH  x2, 2(x1)
        {0x0000A103, 0x100, 0x80FF7F01}, // LW  x2, 0(x1)
        {0x0030C103, 0x100, 0x00000080}, // LBU x2, 3(x1)
        {0x0020D103, 0x100, 0x000080FF}, // LHU x2, 2(x1)
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0xFC;

    memory.write32(0x100, 0x80FF7F01);
    for (const TestCase& test_case : test_cases) {
        target.writeRegister(1, test_case.rs1_value);
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();
        EXPECT_EQ(target.readRegister(2), test_case.expected);
    }
}

TEST(TestCpu, StoreInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t rs2_value;
        uint32_t expected;
    };

    const TestCase test_cases[] = {
        {0x002080A3, 0x100, 0xA1B2C3D4, 0x112233D4}, // SB x2, 1(x1)
        {0x00209123, 0x100, 0xA1B2C3D4, 0x1122C3D4}, // SH x2, 2(x1)
        {0x0020A223, 0x100, 0xA1B2C3D4, 0xA1B2C3D4}, // SW x2, 4(x1)
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0xFC;

    for (const TestCase& test_case : test_cases) {
        const uint32_t destination_address =
            test_case.rs1_value + compute_inmediate(test_case.instruction);
        memory.write32(destination_address, 0x11223344);
        target.writeRegister(1, test_case.rs1_value);
        target.writeRegister(2, test_case.rs2_value);
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();
        EXPECT_EQ(memory.read32(destination_address), test_case.expected);
    }
}

TEST(TestCpu, BranchInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t rs2_value;
        uint32_t expected_pc;
    };

    const TestCase test_cases[] = {
        {0x00208463, 10, 10, 0x108},          // BEQ: equal, branch taken
        {0x00208463, 10, 11, 0x104},          // BEQ: not equal, not taken
        {0x00209463, 10, 11, 0x108},          // BNE: not equal, branch taken
        {0x00209463, 10, 10, 0x104},          // BNE: equal, not taken
        {0x0020C463, 0xFFFFFFFF, 1, 0x108},  // BLT: -1 < 1 (signed), branch taken
        {0x0020C463, 1, 0xFFFFFFFF, 0x104},  // BLT: 1 < -1 (signed), not taken
        {0x0020D463, 0xFFFFFFFF, 1, 0x104},  // BGE: -1 >= 1 (signed), not taken
        {0x0020D463, 1, 0xFFFFFFFF, 0x108},  // BGE: 1 >= -1 (signed), branch taken
        {0x0020E463, 1, 0xFFFFFFFF, 0x108},  // BLTU: 1 < UINT_MAX, branch taken
        {0x0020E463, 0xFFFFFFFF, 1, 0x104},  // BLTU: UINT_MAX < 1, not taken
        {0x0020F463, 0xFFFFFFFF, 1, 0x108},  // BGEU: UINT_MAX >= 1, branch taken
        {0x0020F463, 1, 0xFFFFFFFF, 0x104},  // BGEU: 1 >= UINT_MAX, not taken
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0x100;

    for (const TestCase& test_case : test_cases) {
        target.writeRegister(1, test_case.rs1_value);
        target.writeRegister(2, test_case.rs2_value);
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();

        EXPECT_EQ(target.getPC(), test_case.expected_pc);
    }
}

TEST(TestCpu, JumpInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t rs1_value;
        uint32_t expected_pc;
        uint32_t expected_rd_value;
    };

    const TestCase test_cases[] = {
        {0x008000EF, 0, 0x108, 0x104},     // JAL x1, 8
        {0x004100E7, 0x200, 0x204, 0x104}, // JALR x1, 4(x2)
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0x100;

    for (const TestCase& test_case : test_cases) {
        target.reset();
        target.writeRegister(2, test_case.rs1_value);
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();

        EXPECT_EQ(target.getPC(), test_case.expected_pc);
        EXPECT_EQ(target.readRegister(1), test_case.expected_rd_value);
    }
}

TEST(TestCpu, UpperImmediateInstructions) {
    struct TestCase {
        uint32_t instruction;
        uint32_t expected_rd_value;
    };

    const TestCase test_cases[] = {
        {0x123450B7, 0x12345000}, // LUI x1, 0x12345
        {0x12345097, 0x12345100}, // AUIPC x1, 0x12345
    };

    Memory memory(1024);
    CPU target(memory);
    constexpr uint32_t address = 0x100;

    for (const TestCase& test_case : test_cases) {
        memory.write32(address, test_case.instruction);
        target.setPC(address);
        target.step();

        EXPECT_EQ(target.getPC(), address + 4);
        EXPECT_EQ(target.readRegister(1), test_case.expected_rd_value);
    }
}