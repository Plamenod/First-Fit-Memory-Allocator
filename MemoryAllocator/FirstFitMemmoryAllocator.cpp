#include "FirstFitMemmoryAllocator.h"
#include <Windows.h>
#include <iostream>

const size_t SIZE_HEADER = 13;
const size_t START = 13;

/*
 * @breaf: helper struct which contents info about current chunk
 * @Note: Also every ram page will start with info about the new ram page, and after that every chunk will      * start with the same info header.
 */

struct Header {
    
    Header(uint32_t prev, uint32_t next, uint32_t size, uint8_t isFree)
    {
        this->prev = prev;
        this->next = next;
        this->chunkSize = size;
        this->isFree = isFree;
    }

    uint32_t prev;
    uint32_t next;
    uint32_t chunkSize;
    uint8_t isFree;
};



FirstFitMemmoryAllocator::FirstFitMemmoryAllocator(size_t sizeOfram) :
    ram(static_cast<uint8_t*>(
            VirtualAlloc(
                NULL,
                sizeOfram,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE)))
{
    if(GetProcessHeap() == NULL)
    {
        std::cerr << "ERROR Handle\n";
        exit(1);
    }
    else if(sizeOfram <= SIZE_HEADER)
    {
        std::cout << "ERROR too small size of ram\n";
        exit(1);
    }

    appendFirstHeader(sizeOfram);
}

 void FirstFitMemmoryAllocator::appendFirstHeader(size_t sizeOfram)
 {
    Header startHeader(0, 0, sizeOfram, true);
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

    if(isNotCommite())
    {
        size_t index = START;
        firstMultipleAddress(index);
        uint32_t newSize = bytesToAllocate + index;
        memcpy(&ram[9], &newSize, sizeof(uint32_t));
        ram[8] = false;
        return (void*)index;//static_cast<void*>(index);
    }



}

void FirstFitMemmoryAllocator::firstMultipleAddress(size_t& start)
{
    while(true)
    {
        uint32_t address = (uint32_t)(ram + start);
        bool isMultiple = address % 8 == 0 ? true : false; // CANNOT DO STATIC CAST ???   
        if(isMultiple)
        {
            break;
        }
        ++start;
    }
}


bool FirstFitMemmoryAllocator::isNotCommite()
{
    return ram[8];
}

