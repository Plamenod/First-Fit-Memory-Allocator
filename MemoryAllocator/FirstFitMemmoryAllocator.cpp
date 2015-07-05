#include "FirstFitMemmoryAllocator.h"
#include <Windows.h>
#include <iostream>
#include <bitset>

const size_t SIZE_HEADER = 24;
const size_t START_OF_HEAP = 24;

/*
 * @breaf: helper struct which contents info about current chunk
 * @Note: Also every ram page will start with info about the new ram page, and after that every chunk will
 * start with the same info header.
 */

struct Header {
    
    Header(uint64_t prev, uint64_t next, uint32_t size, bool isFree)
    {
        this->prev = prev;
        this->chunkSize = size;
        this->next = next;
        if(isFree)
        {
            chunkSize |= (1 << 31);
        }
        // TODO check max size of chunk, because used last bite to see whether is free or not
    }

    uint64_t prev;
    uint64_t next;
    uint32_t chunkSize;
};



FirstFitMemmoryAllocator::FirstFitMemmoryAllocator(size_t sizeOfram) :
    ram(static_cast<uint8_t*>(
            VirtualAlloc(
                NULL,
                sizeOfram,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE)))
{
 
    if(sizeOfram <= SIZE_HEADER)
    {
        std::cout << "ERROR too small size of ram\n";
        exit(1);
    }

    appendFirstHeader(sizeOfram);
}

 void FirstFitMemmoryAllocator::appendFirstHeader(size_t sizeOfram)
 {
    Header startHeader(0, 0, sizeOfram - SIZE_HEADER, true);
    memcpy(ram, &startHeader, sizeof(Header));
 }

FirstFitMemmoryAllocator::~FirstFitMemmoryAllocator(void)
{
    // TODO
}

void* FirstFitMemmoryAllocator::allocateBlocks(uint32_t bytesToAllocate)
{
    if(bytesToAllocate == 0)
    {
        return NULL;
    }

    if(isNotCommited())
    {
        uint64_t address = START_OF_HEAP + SIZE_HEADER;
        firstMultipleAddress(address);

        uint32_t newSize = bytesToAllocate + (address - (START_OF_HEAP + SIZE_HEADER));
        setIsUsed(newSize);

         if(getCorrectSize(newSize) > (uint32_t)&ram[8])
        {
            // TODO new heap
            return NULL;
        }

        Header newHeader((uint64_t)ram, 0, newSize, false);
        memcpy(getRamByIndex(START_OF_HEAP), &newHeader, sizeof(Header));
        return static_cast<void*>(&address);
    }




}
uint32_t FirstFitMemmoryAllocator::getCorrectSize(uint32_t newSize)
{
    return newSize & ~(1 << 31);
}


uint8_t* FirstFitMemmoryAllocator::getRamByIndex(uint64_t index)
{
    return &ram[index];
}

void FirstFitMemmoryAllocator::setIsUsed(uint32_t & newSize)
{
    newSize &= ~(1 << 31);
}


void FirstFitMemmoryAllocator::firstFitBlock(
    uint64_t & currentBlock,
    uint64_t & firstFitAddress)
{
    
}


void FirstFitMemmoryAllocator::firstMultipleAddress(uint64_t & start)
{
    uint32_t address = (uint32_t)(ram + start);
    while(true)
    {
        bool isMultiple = address % 8 == 0 ? true : false;  
        if(isMultiple)
        {
            break;
        }
        ++start;
    }
    start = address;
}


bool FirstFitMemmoryAllocator::isNotCommited()
{
    std::bitset<8> tmp((uint8_t) ram[11]);
    std::cout << tmp << std::endl;
    return ram[19] & (1 << 7);
}

