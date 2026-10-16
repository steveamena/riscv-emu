#include "cpu.h"
#include "memory.h"

void CPU::reset() {
    program_counter_ = 0;
    for(int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }
}

void CPU::step(){
    uint32_t instruction = CPU::fetch();
    CPU::Instruction decoded_instruction = CPU::decode(instruction);
    CPU::printInstruction(decoded_instruction);
    CPU::execute(decoded_instruction);
    program_counter_ += 4;
}

uint32_t CPU::fetch(){
    uint32_t instruction = memory.read32(program_counter_);
    return instruction;
}

CPU::Instruction CPU::decode(uint32_t instruction) {
    Instruction inst{};
    inst.opcode = instruction & 0x7F;
    inst.rd = (instruction >> 7) & 0x1F;
    inst.funct3 = (instruction >> 12) & 0x7;
    inst.rs1 = (instruction >> 15) & 0x1F;
    switch(inst.opcode){
        case Opcode::ALU_Reg:{
            inst.rs2 = (instruction >> 20) & 0x1F;
            inst.funct7 = (instruction >> 25) & 0x7F;
            break;
        }
        case Opcode::JALR:
        case Opcode::ALU_Imm:
            inst.imm = static_cast<int32_t>(instruction) >> 20;
            break;
        case Opcode::JAL:{
            // 1. Extract the segments from each register section
            int32_t bit20    = (instruction >> 31) & 0x1;
            int32_t bit19_12 = (instruction >> 12) & 0xFF;
            int32_t bit11    = (instruction >> 20) & 0x1;
            int32_t bit10_1  = (instruction >> 21) & 0x3FF;

            // 2. Join together all registers in the right position. 
            inst.imm = (bit20 << 20) | (bit19_12 << 12) | (bit11 << 11) | (bit10_1 << 1);
            if (inst.imm & 0x100000) { 
                inst.imm |= static_cast<int32_t>(0xFFE00000);
            }
            break;
        }
        case Opcode::Branch:{
            int32_t bit12   = (instruction >> 31) & 0x1;      // Sign
            int32_t bit10_5 = (instruction >> 25) & 0x3F;     // Bits [10:5]
            int32_t bit4_1  = (instruction >> 8) & 0xF;       // Bits [4:1]
            int32_t bit11   = (instruction >> 7) & 0x1;       // Bit [11]
            inst.imm = (bit12 << 12) | (bit11 << 11) | (bit10_5 << 5) | (bit4_1 << 1);
            if (inst.imm & 0x1000) {
                inst.imm |= static_cast<int32_t>(0xFFFFE000);
            }
            break;
        }
        case Opcode::Store:{
            int32_t imm_hi = (instruction >> 25) & 0x7F; // Bits [11:5]
            int32_t imm_lo = (instruction >> 7) & 0x1F;  // Bits [4:0]
            inst.imm = (imm_hi << 5) | imm_lo;

            if (inst.imm & 0x800) {
                inst.imm |= static_cast<int32_t>(0xFFFFF000);
            }
            break;
        }
        default: 
            break;
    }
    return inst;
}

void CPU::printInstruction(Instruction instruction) {
    printf("Opcode: 0x%02X, rd: %u, funct3: 0x%X, rs1: %u, rs2: %u, funct7: 0x%02X\n",
           instruction.opcode,
           instruction.rd,
           instruction.funct3,
           instruction.rs1,
           instruction.rs2,
           instruction.funct7);
}

uint32_t CPU::readRegister(uint32_t index) const {
    return registers[index];
}

void CPU::writeRegister(uint32_t index, uint32_t value){
    if(index == 0 ){
        throw std::domain_error("x0 must be 0");
    }
    registers[index] = value;
}

void CPU::execute(const Instruction& instruction){
    switch (instruction.opcode)
    {
    case Opcode::ALU_Reg:
        switch(instruction.funct3){
            case 0x0:{
                if (instruction.funct7 == 0x0){
                    printf("ADD instruction\n");
                    uint32_t result = readRegister(instruction.rs1) + readRegister(instruction.rs2);
                    writeRegister(instruction.rd, result);
                    printf("Result: %d\n", result);
                }
                if (instruction.funct7 == 0x20){
                    printf("SUB instruction\n");
                    uint32_t result = readRegister(instruction.rs1) - readRegister(instruction.rs2);
                    writeRegister(instruction.rd, result);
                    printf("Result: %d\n", result);
                }
                break;
            }
            case 0x4:{
                printf("XOR instruction\n");
                uint32_t result = readRegister(instruction.rs1) ^ readRegister(instruction.rs2);
                writeRegister(instruction.rd, result);
                printf("Result: %08X\n", result);
                break;
            } 
            case 0x6:{
                printf("OR instruction\n");
                uint32_t result = readRegister(instruction.rs1) | readRegister(instruction.rs2);
                writeRegister(instruction.rd, result);
                printf("Result: %08X\n", result);
                break;
            }
            case 0x7:{
                printf("AND instruction\n");
                uint32_t result = readRegister(instruction.rs1) & readRegister(instruction.rs2);
                writeRegister(instruction.rd, result);
                printf("Result: %08X\n", result);
                break;
            }
            default:
                break;
            }
    case Opcode::ALU_Imm:{
        switch (instruction.funct3){
            case 0x0:{
                printf("ADDI instruction\n");
                uint32_t result = static_cast<int32_t>(readRegister(instruction.rs1)) + instruction.imm;
                writeRegister(instruction.rd, static_cast<uint32_t>(result));
                printf("Result: %d\n", result);
                break;
                }
            case 0x2:{
                printf("LOAD WORD instruction\n");
                uint32_t base = readRegister(instruction.rs1);
                uint32_t address = base + instruction.imm;

                // Read information from ram and store it in the destination register
                uint32_t word = 0;
                writeRegister(instruction.rd, memory.read32(address));
                break;
            }
        }
    }
    case Opcode::JAL:{
        writeRegister(instruction.rd, program_counter_ + 4);
        program_counter_ += instruction.imm; 
        break;
    }
    case Opcode::JALR:{
        writeRegister(instruction.rd, program_counter_ + 4);
        program_counter_ = instruction.rs1 + instruction.imm;
        break;
    }
    case Opcode::Branch:{
        uint32_t val1 = readRegister(instruction.rs1);
        uint32_t val2 = readRegister(instruction.rs2);
        switch(instruction.funct3) {
            case 0x0:{
                if (val1 == val2) {
                    printf("BRANCH EQUAL instruction\n");
                    program_counter_ = instruction.imm;
                    return;
                }
            }
            case 0x1:{
                if (val1 != val2) {
                    printf("BRANCH NOT EQUAL instruction\n");
                    program_counter_ = instruction.imm;
                    return;
                }
            }
        }
    }
    case Opcode::Store:{
        printf("STORE WORD instruction\n");
        uint32_t base = readRegister(instruction.rs1);
        uint32_t address = base + instruction.imm;
        uint32_t word = instruction.rs2;
        memory.write32(address, word);
        break;
    }
    default:
        break;
    }


}

void CPU::print32int(uint32_t value){
    printf("%02X %02X %02X %02X\n", value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF, (value >> 24) & 0xFF);
}
