#include <cstdio>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))

struct DFSinfo {
    word_t exclude; // frame not to be considered empty even if it has only 0s
    uint64_t page_to_create;
    word_t *empty_frame;
    word_t *max_frame_index; //
    uint64_t *pageToEvict; // VM page in PM with greatest cyclical difference from page_to_create
    uint64_t *frameToEvictFrom;
    uint64_t *locOfEvictFrame;
} typedef DFSinfo;

void DFShelper(word_t frame, uint64_t parent, uint64_t curPage, int depth, DFSinfo *info);
void DFS(DFSinfo * info);
uint64_t cyclicDist(uint64_t a, uint64_t b);

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
        word_t empty_frame = 0;
        word_t max_frame_index = 0;
        uint64_t pageToEvict = virtualAddress >> OFFSET_WIDTH;
        uint64_t frameToEvictFrom;
        uint64_t locOfEvictFrame;
        uint64_t page_to_create = virtualAddress >> OFFSET_WIDTH;
        DFSinfo info = {parent_frame, page_to_create, &empty_frame, &max_frame_index, &pageToEvict, &frameToEvictFrom, &locOfEvictFrame};

        if (child_frame) {
            parent_frame = child_frame;
            continue;
        }

        DFS(&info);
        child_frame = empty_frame;

        // no empty frames available, try using unused frame
        if (child_frame == 0 && max_frame_index + 1 < NUM_FRAMES) {
            child_frame = max_frame_index + 1;
        }

        // all frames are used and none are empty, we must evict a page.
        if (child_frame == 0) {
            PMevict(*(info.frameToEvictFrom), *(info.pageToEvict));
            for (int j = 0; j < PAGE_SIZE; ++j) {
                PMwrite((*(info.frameToEvictFrom) * PAGE_SIZE) + j, 0);
            }
            PMwrite(locOfEvictFrame, 0);
            child_frame = *(info.frameToEvictFrom);
        }
        PMwrite((parent_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), child_frame);
        parent_frame = child_frame;
    }

    uint64_t page_to_restore = virtualAddress >> OFFSET_WIDTH;
    PMrestore(child_frame, page_to_restore);
    PMread((child_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress), value);

    return 0;
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    word_t parent_frame = 0;
    word_t child_frame;
    unsigned int curBits;
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        curBits = VIRTUAL_ADDRESS_WIDTH - ( (i + 1) * OFFSET_WIDTH);
        PMread((parent_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), &child_frame);

        // page is not in physical memory, check for empty frames
        word_t empty_frame = 0;
        word_t max_frame_index = 0;
        uint64_t pageToEvict = virtualAddress >> OFFSET_WIDTH;
        uint64_t frameToEvictFrom;
        uint64_t locOfEvictFrame;
        uint64_t page_to_create = virtualAddress >> OFFSET_WIDTH;
        DFSinfo info = {parent_frame, page_to_create, &empty_frame, &max_frame_index, &pageToEvict, &frameToEvictFrom, &locOfEvictFrame};

        if (child_frame) {
            parent_frame = child_frame;
            continue;
        }

        DFS(&info);
        child_frame = empty_frame;

        // no empty frames available, try using unused frame
        if (child_frame == 0 && max_frame_index + 1 < NUM_FRAMES) {
            child_frame = max_frame_index + 1;
        }

        // all frames are used and none are empty, we must evict a page.
        if (child_frame == 0) {
            PMevict(*(info.frameToEvictFrom), *(info.pageToEvict));
            for (int j = 0; j < PAGE_SIZE; ++j) {
                PMwrite((*(info.frameToEvictFrom) * PAGE_SIZE) + j, 0);
            }
            PMwrite(locOfEvictFrame, 0);
            child_frame = *(info.frameToEvictFrom);
        }
        PMwrite((parent_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress >> curBits), child_frame);
        parent_frame = child_frame;
    }
    uint64_t page_to_restore = virtualAddress >> OFFSET_WIDTH;
    PMrestore(child_frame, page_to_restore);
    PMwrite((child_frame * PAGE_SIZE) + RIGHTMOST_BITS(virtualAddress), value);

    return 0;
}


void DFShelper(word_t frame, uint64_t parent, uint64_t curPage, int depth, DFSinfo *info)
{

    if (depth == TABLES_DEPTH) {
        if (cyclicDist(curPage, info->page_to_create) > cyclicDist(*(info->pageToEvict), info->page_to_create)) {
            *(info->frameToEvictFrom) = frame;
            *(info->pageToEvict) = curPage;
            *(info->locOfEvictFrame) = parent;
        }
        return;
    }

    word_t holder1 = 0;
    // this is where the magic happens
    for (int i = 0; i < PAGE_SIZE; ++i) {
        word_t holder2;
        PMread((frame * PAGE_SIZE) + i, &holder2);
        if (holder2 > 0) {
            // update max_frame_index
            if (holder2 > *(info->max_frame_index)) {
                *(info->max_frame_index) = holder2;
            }
            // continue DFS looking for empty frame below
            uint64_t nextPage = (curPage << OFFSET_WIDTH) + i;
            DFShelper(holder2, (frame * PAGE_SIZE) + i, nextPage, depth + 1, info);
        holder1 += holder2;
        }
    }

    if (holder1 == 0 && frame != info->exclude && *(info->empty_frame) == 0) {
        // the frame given as an argument points to all zeroes -- remove it from its parent and save it.
        PMwrite(parent, 0);
        *(info->empty_frame) = frame;
    }
}

void DFS(DFSinfo * info)
{
    DFShelper(0, 0, 0, 0, info);
}




uint64_t cyclicDist(uint64_t a, uint64_t b) {
    if (b < a) {
        uint64_t temp = a;
        a = b;
        b = temp;
    }

    uint64_t distance = b - a;
    if (NUM_PAGES - distance < distance) {
        distance = NUM_PAGES - distance;
    }

    return distance;

}

/*int main() {
    VMinitialize();
    VMwrite(37293038928, 67);
    word_t holder;
    VMread(37293038928, &holder);
    printf("%d\n", holder);
    return 0;
}*/
