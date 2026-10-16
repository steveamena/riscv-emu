#include <cstdint>
#include <iostream>

#include "memory.h"

namespace Opcode {
    constexpr uint8_t ALU_Reg   = 0x33;
    constexpr uint8_t ALU_Imm   = 0x13;
    constexpr uint8_t Load      = 0x03;
    constexpr uint8_t Store     = 0x23;
    constexpr uint8_t Branch    = 0x63;
    constexpr uint8_t JAL       = 0x6F;
    constexpr uint8_t JALR      = 0x67;
    constexpr uint8_t LUI       = 0x37;
    constexpr uint8_t AUIPC     = 0x17;
    constexpr uint8_t System    = 0x73;
}

class CPU {
    public:
        struct Instruction {
            uint32_t opcode;
            uint32_t rd;
            uint32_t funct3;
            uint32_t rs1;
            uint32_t rs2;
            uint32_t funct7;
            int32_t imm;
        };

        CPU(Memory& mem) : memory(mem){
            reset();
        }
        void reset();
        void step();


        // methods tied to registers readings
        uint32_t readRegister(uint32_t index) const;
        void writeRegister(uint32_t index, uint32_t value);

        // auxiliary methods
        void setPC(uint32_t new_pc) { program_counter_ = new_pc; }
        void printInstruction(Instruction instruction);
        void print32int(uint32_t value);
    private:
        // fetch -> decode -> execute methods
        uint32_t fetch();
        Instruction decode(uint32_t instruction);
        void execute(const Instruction& instruction);

        uint32_t registers[32];
        uint32_t program_counter_;

        Memory& memory;
};