
#include "memory.h"

Memory::Memory(size_t size) : data_(size) {};

std::vector<uint8_t> Memory::data(){
    return data_;
}

void Memory::write8(uint32_t address, uint8_t value) {
    if(address >= data_.size()) {
        throw std::out_of_range("Address out of range");
    }
    data_[address] = value;
}

uint8_t Memory::read8(uint32_t address) const {
    if(address >= data_.size()) {
        throw std::out_of_range("Address out of range");
    }
    return data_[address];
}

int Memory::read8Int(uint32_t address) const {
    return static_cast<int>(read8(address));
}

void Memory::write32(uint32_t address, uint32_t value) {
    if(address + 3 >= data_.size()) {
        throw std::out_of_range("Address out of range");
    }
    data_[address] = value & 0xFF;
    data_[address + 1] = (value >> 8) & 0xFF;
    data_[address + 2] = (value >> 16) & 0xFF;
    data_[address + 3] = (value >> 24) & 0xFF;
}

uint32_t Memory::read32(uint32_t address) const {
    if(address + 3 >= data_.size()) {
        throw std::out_of_range("Address out of range");
    }
    return static_cast<uint32_t>(data_[address]) |
           (static_cast<uint32_t>(data_[address + 1]) << 8) |
           (static_cast<uint32_t>(data_[address + 2]) << 16) |
           (static_cast<uint32_t>(data_[address + 3]) << 24);
}

int Memory::read32Int(uint32_t address) const {
    return static_cast<int>(read32(address));
}

void Memory::print32Hex(uint32_t address) const {
    if(address + 3 >= data_.size()) {
        throw std::out_of_range("Address out of range");
    }

    printf("% % % %\n", 
           static_cast<unsigned char>(data_[address]),
           static_cast<unsigned char>(data_[address + 1]),
           static_cast<unsigned char>(data_[address + 2]),
           static_cast<unsigned char>(data_[address + 3]));
}

