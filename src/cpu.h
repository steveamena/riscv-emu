#include <cstdint>
#include <array>

#include "memory.h"

enum class Opcode : uint8_t {
    ALU_Reg = 0x33,
    ALU_Imm = 0x13,
    Load = 0x03,
    Store = 0x23,
    Branch = 0x63,
    JAL = 0x6F,
    JALR = 0x67,
    LUI = 0x37,
    AUIPC = 0x17,
    System = 0x73
};

class CPU {
    public:
        struct Instruction {
            Opcode opcode;
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
        uint32_t getPC() const { return program_counter_; }
        void printInstruction(Instruction instruction);
        void print32int(uint32_t value);
        int32_t sign_extend(uint32_t value, uint32_t size);
    private:
        // fetch -> decode -> execute methods
        uint32_t fetch();
        Instruction decode(uint32_t instruction);
        uint32_t execute(const Instruction& instruction, const uint32_t current_pc);
        static constexpr size_t register_size_ = 32;
        std::array<uint32_t, register_size_> registers;
        uint32_t program_counter_;
        Memory& memory;
};