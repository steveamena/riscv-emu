#include "memory.h"

Memory::Memory(size_t size) : data(size) {};

void Memory::write8(uint32_t address, uint8_t value) {
    if(address >= data.size()) {
        throw std::out_of_range("Address out of range");
    }
    data[address] = value;
}

uint8_t Memory::read8(uint32_t address) const {
    return data[address];
}

int Memory::read8Int(uint32_t address) const {
    return static_cast<int>(read8(address));
}

void Memory::write32(uint32_t address, uint32_t value) {
    if(address + 3 >= data.size()) {
        throw std::out_of_range("Address out of range");
    }
    data[address] = value & 0xFF;
    data[address + 1] = (value >> 8) & 0xFF;
    data[address + 2] = (value >> 16) & 0xFF;
    data[address + 3] = (value >> 24) & 0xFF;
}

uint32_t Memory::read32(uint32_t address) const {
    if(address + 3 >= data.size()) {
        throw std::out_of_range("Address out of range");
    }
    return static_cast<uint32_t>(data[address]) |
           (static_cast<uint32_t>(data[address + 1]) << 8) |
           (static_cast<uint32_t>(data[address + 2]) << 16) |
           (static_cast<uint32_t>(data[address + 3]) << 24);
}

void Memory::print32Hex(uint32_t address) const {
    if(address + 3 >= data.size()) {
        throw std::out_of_range("Address out of range");
    }

    printf("% % % %\n", 
           static_cast<unsigned char>(data[address]),
           static_cast<unsigned char>(data[address + 1]),
           static_cast<unsigned char>(data[address + 2]),
           static_cast<unsigned char>(data[address + 3]));
}

