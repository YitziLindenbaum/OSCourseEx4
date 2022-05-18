#include <cstdlib>
#include <iostream>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))


void VMinitialize() {
    for (int i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(i, 0);
    }
}


int VMread(uint64_t virtualAddress, word_t* value) {
    word_t address = 0; //virtualAddress >> (VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH);
    unsigned int curBits;
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        curBits = VIRTUAL_ADDRESS_WIDTH - ( (i + 1) * OFFSET_WIDTH);
        PMread((address * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), &address);
    }
    return 0;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    return 0;
}

int main() {
    uint64_t num = std::rand();
    std::cout << num << std::endl << RIGHTMOST_BITS(num) << std::endl;
    return 0;
}