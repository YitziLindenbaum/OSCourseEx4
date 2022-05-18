//
// Created by Yitzchak Lindenbaum  on 18/05/2022.
//
#include <cstdlib>
#include <iostream>
#include "MemoryConstants.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))

int main() {
    time_t t;
    srand((unsigned) time(&t));
    uint64_t num = rand();
    std::cout << num << std::endl << RIGHTMOST_BITS(num) << std::endl;
    return 0;
}