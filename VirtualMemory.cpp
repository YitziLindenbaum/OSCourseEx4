#include <cstdio>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))


word_t findEmptyFrameBelow(word_t frame, uint64_t parent, word_t exclude, word_t *max_frame_index, int depth);
word_t findEmptyFrame(word_t exclude, word_t *max_frame_index);

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

        // page is not in physical memory, check for empty frames
        word_t max_frame_index;
        if (child_frame == 0) {
            child_frame = findEmptyFrame(parent_frame, &max_frame_index);
        }

        // no empty frames available, try using unused frame
        if (child_frame == 0 && max_frame_index + 1 < NUM_FRAMES) {
            child_frame = max_frame_index + 1;
        }

        // all frames are used and none are empty, we must evict a page.
        if (child_frame == 0) {
            // TODO continue here
        }
        PMwrite((parent_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), child_frame);
        parent_frame = child_frame;
        // @todo in theory, two addresses can be unified into the same variable?
    }
    PMread((child_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress), value);

    return 0;
}

/**
 * Recursive helper function for findEmptyFrame -- finds an empty frame below the given one in the VM tree,
 * if one exists.
 * @param frame The frame from which to start DFS
 * @param depth Level of the tree the starting frame is in
 * @param exclude Frame to exclude (not to be considered empty)
 * @param max_frame_index Output argument -- maximal index of a used frame.
 * @return Index of an empty frame, or 0 if none exist.
 */
word_t findEmptyFrameBelow(word_t frame, uint64_t parent, word_t exclude, word_t *max_frame_index, unsigned int depth) {

    if (depth == TABLES_DEPTH || frame == exclude) {
        return 0;
    }

    word_t holder1 = 0;

    for (int i = 0; i < PAGE_SIZE; ++i) {
        word_t holder2;
        PMread((frame * PAGE_SIZE) + i, &holder2);
        if (holder2 > 0) {
            if (holder2 > *max_frame_index) {*max_frame_index = holder2;}
            word_t emptyFrame = findEmptyFrameBelow(holder2, (frame * PAGE_SIZE) + i, exclude, max_frame_index, depth + 1);
            if (emptyFrame > 0) {
                return emptyFrame;
            }
        holder1 += holder2;
        }
    }
    if (holder1 > 0) {
        return 0;
    }
    // the frame given as an argument points to all zeros -- remove it from its parent and return it.
    PMwrite(parent, 0);
    return frame;
}

word_t findEmptyFrame(word_t exclude, word_t *max_frame_index) {
    return findEmptyFrameBelow(0, 0, exclude, max_frame_index, 0);
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
