//
// Created by Yitzchak Lindenbaum  on 18/05/2022.
//
#include <cstdlib>
#include <iostream>
#include "MemoryConstants.h"

#define RIGHTMOST_BITS(num) (num & (PAGE_SIZE - 1))

void add_one(word_t *ptr) {
    (*ptr)++;
}

int main() {
    word_t num = 4;
    std::cout << num << std::endl;
    add_one(&num);
    std::cout << num << std::endl;
    return 0;
}