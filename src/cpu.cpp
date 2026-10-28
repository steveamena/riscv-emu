#include "cpu.h"
#include "memory.h"

void CPU::reset() {
    program_counter_ = 0;
    for(int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }
}

void CPU::step(){
    const uint32_t current_pc = program_counter_;
    uint32_t instruction = CPU::fetch();
    CPU::Instruction decoded_instruction = CPU::decode(instruction);
    CPU::printInstruction(decoded_instruction);
    program_counter_ = CPU::execute(decoded_instruction, current_pc);
}

uint32_t CPU::fetch(){
    uint32_t instruction = memory.read32(program_counter_);
    return instruction;
}

CPU::Instruction CPU::decode(uint32_t instruction) {
    Instruction inst{};
    inst.opcode = static_cast<Opcode>(instruction & 0x7F);
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
        case Opcode::Load:
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
            inst.rs2 = (instruction >> 20) & 0x1F;
            const uint32_t immediate = (((instruction >> 31) & 0x1) << 12) |
                                       (((instruction >> 7) & 0x1) << 11) |
                                       (((instruction >> 25) & 0x3F) << 5) |
                                       (((instruction >> 8) & 0xF) << 1);
            inst.imm = (immediate & 0x1000)
                ? static_cast<int32_t>(immediate) - 0x2000
                : static_cast<int32_t>(immediate);
            break;
        }
        case Opcode::Store:{
            int32_t imm_hi = (instruction >> 25) & 0x7F; // Bits [11:5]
            int32_t imm_lo = (instruction >> 7) & 0x1F;  // Bits [4:0]
            inst.imm = (imm_hi << 5) | imm_lo;
            if (inst.imm & 0x800) {
                inst.imm |= static_cast<int32_t>(0xFFFFF000);
            }
            inst.rs2 = (instruction >> 20) & 0x1F;
            break;
        }
        default: 
            break;
    }
    return inst;
}

void CPU::printInstruction(Instruction instruction) {
    printf("Opcode: 0x%02X, rd: %u, funct3: 0x%X, rs1: %u, rs2: %u, funct7: 0x%02X\n",
           static_cast<unsigned>(instruction.opcode),
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
        return;
    }
    registers[index] = value;
}

uint32_t CPU::execute(const Instruction& instruction, const uint32_t current_pc){
    const uint32_t sequential_pc = current_pc + 4;

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
        break;
    case Opcode::ALU_Imm:{
        switch (instruction.funct3){
            case 0x0:{
                printf("ADDI instruction\n");
                uint32_t result = static_cast<int32_t>(readRegister(instruction.rs1)) + instruction.imm;
                writeRegister(instruction.rd, static_cast<uint32_t>(result));
                printf("Result: %d\n", result);
                break;
                }
        }
        break;
    }
    case Opcode::JAL:{
        printf("JAL instruction\n");
        if(instruction.rd !=0){
            writeRegister(instruction.rd, sequential_pc); // In RiscV, JAL allows rd equal to 0 but it is skipped. 
        }
        return current_pc + static_cast<uint32_t>(instruction.imm);
    }
    case Opcode::JALR:{
        if(instruction.rd != 0){
            writeRegister(instruction.rd, sequential_pc); // In RiscV, JALR allows rd equal to 0 but it is skipped.
        }
        return  readRegister(instruction.rs1) + static_cast<uint32_t>(instruction.imm);
    }
    case Opcode::Store:{
        uint32_t base = readRegister(instruction.rs1);
        uint32_t address = base + instruction.imm;
        uint32_t word = readRegister(instruction.rs2);
        switch(instruction.funct3)
        {
            case 0x0:
                printf("STORE Byte instruction\n");
                memory.write8(address, word);
                break;
            case 0x1:
                printf("STORE Half instruction\n");
                memory.write8(address, word);
                memory.write8(address + 1, word);
                break;                  
            case 0x2:
                printf("STORE WORD instruction\n");
                memory.write32(address, word);
                break;
            break;
            default:
            break;
        }
        break;
    }
    case Opcode::Branch: {
        bool branch_condition_met;

        switch (instruction.funct3) {
        case 0x0: // BEQ
            branch_condition_met = readRegister(instruction.rs1) ==
                                   readRegister(instruction.rs2);
            break;
        case 0x1: // BNE
            branch_condition_met = readRegister(instruction.rs1) !=
                                   readRegister(instruction.rs2);
            break;
        case 0x4: // BLT
            branch_condition_met = readRegister(instruction.rs1) <
                                   readRegister(instruction.rs2);
            break;
        case 0x5: // BGE
            branch_condition_met = readRegister(instruction.rs1) >=
                                   readRegister(instruction.rs2);
            break;
        case 0x6: // BLTU (unsigned)
            branch_condition_met = readRegister(instruction.rs1) <
                                  readRegister(instruction.rs2);
            break;
        case 0x7: // BGEU (unsigned)
            branch_condition_met = readRegister(instruction.rs1) >=
                                  readRegister(instruction.rs2);
            break;
        default:
            throw std::runtime_error("Unsupported branch funct3");
        }

        return branch_condition_met
            ? current_pc + static_cast<uint32_t>(instruction.imm)
            : sequential_pc;
    }

    case Opcode::Load:{
        switch (instruction.funct3)
        {
        case 0x0:
        {
            printf("LOAD Byte instruction\n");
            uint32_t byte =
                memory.read8(readRegister(instruction.rs1) + instruction.imm);
            // LB sign-extends: for 0x80, set the upper 24 bits to get 0xFFFFFF80.
            if ((byte & 0x80) != 0) {
                byte |= 0xFFFFFF00;
            }
            writeRegister(instruction.rd, byte);
            break;
        }
        case 0x1:
        {
            printf("LOAD half instruction\n");
            uint32_t half = (memory.read8(readRegister(instruction.rs1) + instruction.imm) << 8
                            | (memory.read8(readRegister(instruction.rs1) + instruction.imm + 1)));
            // LB sign-extends: for 0x80, set the upper 24 bits to get 0xFFFFFF80.
            if ((half & 0x8000) != 0) {
                half |= 0xFFFF0000;
            }                            
            writeRegister(instruction.rd, half);
            break;            
        }
        case 0x2:
            printf("LOAD Word instruction\n");
            writeRegister(instruction.rd, memory.read32(readRegister(instruction.rs1) + instruction.imm));
            break;
        case 0x4:
        {
            printf("LOAD U Byte instruction\n");
            const uint32_t byte =
                memory.read8(readRegister(instruction.rs1) + instruction.imm);
            writeRegister(instruction.rd, byte);
            break;
        }
        case 0x5:
        {
            printf("LOAD half instruction\n");
            const uint32_t half = (memory.read8(readRegister(instruction.rs1) + instruction.imm) << 8
                            | (memory.read8(readRegister(instruction.rs1) + instruction.imm + 1)));                          
            writeRegister(instruction.rd, half);
            break;
        }
        default:
            break;
        }
    }
    default:
        break;
    }

    return sequential_pc;
}

void CPU::print32int(uint32_t value){
    printf("%02X %02X %02X %02X\n", value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF, (value >> 24) & 0xFF);
}
