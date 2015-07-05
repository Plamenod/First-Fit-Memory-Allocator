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
    bool isNotCommited();
    void firstMultipleAddress(uint64_t & start);
    void firstFitBlock(uint64_t & currentBlock, uint64_t & firstFitAddress);
    void setIsUsed(uint32_t & newSize);
    uint8_t* getRamByIndex(uint64_t index);
    uint32_t getCorrectSize(uint32_t newSize); 
};

