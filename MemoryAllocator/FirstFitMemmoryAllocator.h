#pragma once
#include <stdint.h>
#include <memory>

const size_t SIZE_OF_RAM = 1000;//1073741824;

struct Header;

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
    size_t firstMultipleAddressByIndex(uint64_t & index);
    size_t firstMultipleAddressByAddress(uint64_t & address);
    void firstFitBlock(uint64_t & currentBlock, uint64_t & firstFitAddress);
    void setIsUsed(uint32_t & newSize);
    uint8_t* getRamByIndex(uint64_t index);
    uint32_t getCorrectSize(uint32_t newSize);
    Header* nextFreeBlock(uint32_t bytesToAllocate);
    void* split(Header* current,uint32_t bytesToAllocate);
    Header* getCurrentHeap(uint64_t indexOfAddress);
    uint64_t getLastAddress(Header* heap);
};

