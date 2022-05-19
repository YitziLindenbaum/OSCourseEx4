#include <cstdio>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))


void VMinitialize() {
    for (int i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(i, 0);
    }
}


int VMread(uint64_t virtualAddress, word_t* value) {
    word_t old_address = 0;
    word_t new_address;
    unsigned int curBits;
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        curBits = VIRTUAL_ADDRESS_WIDTH - ( (i + 1) * OFFSET_WIDTH);
        PMread((old_address * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), &new_address);
        old_address = new_address;
        // @todo in theory, two addresses can be unified into the same variable?
    }
    PMread((new_address * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress), value);

    return 0;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    word_t old_address = 0;
    word_t new_address;
    unsigned int curBits;
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        curBits = VIRTUAL_ADDRESS_WIDTH - ( (i + 1) * OFFSET_WIDTH);
        PMread((old_address * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), &new_address);
        old_address = new_address;
        // @todo in theory, two addresses can be unified into the same variable?
    }

    PMwrite((new_address * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress), value);
    return 0;
}

int main() {
    VMinitialize();
    VMwrite(37293038928, 67);
    word_t holder;
    VMread(37293038928, &holder);
    printf("%d\n", holder);
    return 0;
}
