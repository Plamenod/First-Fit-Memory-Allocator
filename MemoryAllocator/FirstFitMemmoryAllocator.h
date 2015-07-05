#pragma once
#include <stdint.h>
#include <memory>

const size_t SIZE_OF_RAM = 100;//1073741824;

class FirstFitMemmoryAllocator
{
public:
    FirstFitMemmoryAllocator(size_t sizeOfram = SIZE_OF_RAM);
    ~FirstFitMemmoryAllocator(void); // TODO

    void* allocateBlocks(uint32_t bytesToAllocate);

private:
    uint8_t* ram;

    void splitBlock();
    void mergeBlocks();

    
    void freeBlocks();

    void appendFirstHeader(size_t sizeOfram);
    bool isNotCommite();
    void firstMultipleAddress(size_t& start);
};

