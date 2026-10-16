#include <iostream>
#include "memory.h"
#include "cpu.h"

int main()
{
    Memory memory(1024);
    CPU cpu(memory);
    
    memory.write8(100,42);
    std::cout << memory.read8Int(100) << "\n";
    memory.print32Hex(200);
    cpu.writeRegister(1, 0xA);
    cpu.writeRegister(2, 0x14);
    cpu.setPC(200);


    // Setup memory for testing
        // ADD test
    memory.write32(200,0x002081B3);
    // SUB test
    memory.write32(204,0x402081B3);
    
    // XOR test
    memory.write32(208,0x0020C1B3);

    // OR test
    memory.write32(212,0x0020E1B3);
    
    // AND test
    memory.write32(216,0x0020F1B3);

    // ADDI test
    memory.write32(220,0x00A08193); 
    for(int i=0; i<6; i++){  
        cpu.step();
    }
    return 0;
}
