#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstdio>
#include <cstddef>

class Memory {
    private:
        std::vector<uint8_t> data_;
    public:
        Memory(size_t size);
        std::vector<uint8_t> data();
        
        uint8_t read8(uint32_t address) const;
        void write8(uint32_t address, uint8_t value);
        int read8Int(uint32_t address) const;
        uint32_t read32(uint32_t address) const;
        void write32(uint32_t address, uint32_t value);
        int read32Int(uint32_t address) const;
        void print32Hex(uint32_t address) const;

};
