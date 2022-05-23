#include <cstdio>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))


word_t findEmptyFrameBelow(word_t frame, int depth, word_t exclude);
word_t findEmptyFrame(word_t exclude);

void VMinitialize() {
    for (int i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(i, 0);
    }
}


int VMread(uint64_t virtualAddress, word_t* value) {
    word_t parent_frame = 0;
    word_t child_frame;
    unsigned int curBits;
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        curBits = VIRTUAL_ADDRESS_WIDTH - ( (i + 1) * OFFSET_WIDTH);
        PMread((parent_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), &child_frame);

        if (child_frame == 0) {
            child_frame = findEmptyFrame(parent_frame);
        }
        if (child_frame < 0) {
            // TODO continue here
        }

        parent_frame = child_frame;
        // @todo in theory, two addresses can be unified into the same variable?
    }
    PMread((child_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress), value);

    return 0;
}

word_t findEmptyFrameBelow(word_t frame, int depth, word_t exclude) {

    if (depth == TABLES_DEPTH || frame == exclude) {
        return -1;
    }

    word_t holder1 = 0;
    for (int i = 0; i < PAGE_SIZE; ++i) {
        word_t holder2;
        PMread((frame * PAGE_SIZE) + i, &holder2);
        if (holder2 != 0) {
            word_t emptyFrame = findEmptyFrameBelow(holder2, depth + 1, 0);
            if (emptyFrame > 0) {
                return emptyFrame;
            }
        holder1 += holder2;
        }
    }
    if (holder1 != 0) {
        return -1;
    }
    return frame;
}

word_t findEmptyFrame(word_t exclude) {
    return findEmptyFrameBelow(0, 0, exclude);
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
