#include "FirstFitMemmoryAllocator.h"
#include <Windows.h>
#include <iostream>

FirstFitMemmoryAllocator::FirstFitMemmoryAllocator(size_t sizeOfram) :
    ram(static_cast<uint8_t*>(
            VirtualAlloc(
                NULL,
                sizeOfram,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE)))
{
 
    if(sizeOfram <= 2 * SIZE_HEADER)
    {
        std::cout << "ERROR too small size of ram\n";
        exit(1);
    }

    if(!ram)
    {
        throw std::exception("ERROR Can't allocate memory !!!\n");
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
    size_t size = getCorrectSize(reinterpret_cast<Header*>(ram)->chunkSize);
    VirtualFree(ram, size, MEM_RELEASE);
}

void* FirstFitMemmoryAllocator::myMalloc(uint32_t bytesToAllocate)
{
    if(bytesToAllocate == 0)
    {
        return NULL;
    }

    if(isNotCommited())
    {
        uint64_t address = START_OF_HEAP + SIZE_HEADER;
        uint8_t skip = firstMultipleAddressByIndex(address);

        uint8_t* newAddress = ram;
        newAddress += 2 * SIZE_HEADER + skip;
        uint32_t newSize = bytesToAllocate + skip + SIZE_HEADER;
        setIsUsed(newSize);

        if(getCorrectSize(newSize) > getCorrectSize(*reinterpret_cast<uint64_t*>(getRamByIndex(16)))) 
        {
            // TODO new heap
            return NULL;
        }

        Header newHeader(
            reinterpret_cast<uint64_t>(ram),
            reinterpret_cast<uint64_t>(ram) + SIZE_HEADER + newSize,
            newSize,
            false);
        memcpy(getRamByIndex(START_OF_HEAP), &newHeader, sizeof(Header));

        ram[19] &= ~(1 << 7);
        Header* startHeader = reinterpret_cast<Header*>(ram);
        Header newFreeHeader(
            reinterpret_cast<uint64_t>(ram) + SIZE_HEADER,
            0,
            getCorrectSize(startHeader->chunkSize) - newSize - SIZE_HEADER,
            true);
        memcpy(reinterpret_cast<uint64_t*>(newHeader.next), &newFreeHeader, sizeof(Header));
        return newAddress;
    }

    Header* current = nextFreeBlock(bytesToAllocate);
    return split(current, bytesToAllocate);
}

void* FirstFitMemmoryAllocator::split(Header* current,uint32_t bytesToAllocate)
{
    uint64_t currentAddress = reinterpret_cast<uint64_t>(current) + SIZE_HEADER;

    size_t padding = firstMultipleAddressByAddress(currentAddress);
    uint32_t newSize = SIZE_HEADER + padding + bytesToAllocate;
    uint8_t* newAddress = reinterpret_cast<uint8_t*>(current) + SIZE_HEADER + padding;
    Header occurHeader(
        current->prev,
        reinterpret_cast<uint64_t>(current) + newSize,
        newSize,
        false);
    uint32_t freeSize;
    if(current->next == 0)
    {
        Header* currentHeap = getCurrentHeap(reinterpret_cast<uint64_t>(current));
        uint64_t lastAddress = getLastAddress(currentHeap);
        freeSize = lastAddress - (reinterpret_cast<uint64_t>(current) + newSize + 1);
    }
    else
    {
        freeSize = current->next - reinterpret_cast<uint64_t>(current) + newSize + 1;
    }
    
    Header newFreeHeader(
        reinterpret_cast<uint64_t>(current),
        current->next,
        freeSize,
        true);
    memcpy(reinterpret_cast<uint64_t*>(current) + newSize, &newFreeHeader, sizeof(Header));
    memcpy(current, &occurHeader, sizeof(Header));
    return newAddress;
}

uint64_t FirstFitMemmoryAllocator::getLastAddress(Header* heap)
{
    return reinterpret_cast<uint64_t>(heap) + heap->chunkSize - 1;
}

Header* FirstFitMemmoryAllocator::nextFreeBlock(uint32_t bytesToAllocate)
{
    Header* currentHeader = reinterpret_cast<Header*>(getRamByIndex(START_OF_HEAP));

    while(getCorrectSize(currentHeader->chunkSize) >=
        (bytesToAllocate + SIZE_HEADER) &&
        !currentHeader->isFree())
    {
        currentHeader = reinterpret_cast<Header*>(currentHeader->next);
        if(currentHeader == NULL)
        {
            // TODO allocate new heap
        }

    }

    if(getCorrectSize(currentHeader->chunkSize) <
        (bytesToAllocate + SIZE_HEADER) && currentHeader->next == 0)
    {
        // TODO make new heap
        return NULL;
    }

    return currentHeader;
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


size_t FirstFitMemmoryAllocator::firstMultipleAddressByIndex(uint64_t & start)
{
    Header* currentHeap = getCurrentHeap(reinterpret_cast<uint64_t>(getRamByIndex(start)));
    if(currentHeap == NULL)
    {
        std::cerr << "WRONG ADDRESS\n";
        exit(1);
    }

    uint32_t address = reinterpret_cast<uint32_t>(currentHeap) + start;
    size_t numberOfSkipedAddresses = 0;
    while(true)
    {
        bool isMultiple = address % 8 == 0 ? true : false;  
        if(isMultiple)
        {
            break;
        }
        ++start;
        ++numberOfSkipedAddresses;
    }
    start = address;
    return numberOfSkipedAddresses;
}

size_t FirstFitMemmoryAllocator::firstMultipleAddressByAddress(uint64_t & address)
{
// TODO check whether it's in range, when added multiple heap functionality
    size_t numberOfSkipedAddresses = 0;
    while(true)
    {
        bool isMultiple = address % 8 == 0 ? true : false;  
        if(isMultiple)
        {
            break;
        }
        ++address;
        ++numberOfSkipedAddresses;
    }

    return numberOfSkipedAddresses;
}


Header* FirstFitMemmoryAllocator::getCurrentHeap(uint64_t indexOfAddress)
{
    Header* iterHeap = (Header*)ram;
    do
    {
        uint64_t firstAddress = reinterpret_cast<uint64_t>(iterHeap);
        uint64_t lastAddress = firstAddress + getCorrectSize(iterHeap->chunkSize);
        if(firstAddress <= indexOfAddress && indexOfAddress <= lastAddress)
        {
            return iterHeap;
        }
        else
        {
            if(iterHeap->next)
            {
                iterHeap = reinterpret_cast<Header*>(iterHeap->next);
            }
            else
            {
                break;
            }

        }
    }
    while(iterHeap);
    return iterHeap;
}

bool FirstFitMemmoryAllocator::isNotCommited()
{
    return ram[19] & (1 << 7);
}

void FirstFitMemmoryAllocator::myFree(void* addressToFree)
{
    Header* address = getHeaderByAddress(addressToFree);
    if(!address)
    {
        std::cerr << "Can't myFree " << addressToFree << std::endl;
        return;
    }
    merge(address);
}

 Header* FirstFitMemmoryAllocator::getHeaderByAddress(void* addressToFree)
 {
    Header* currentHeap = getCurrentHeap(reinterpret_cast<uint64_t>(addressToFree));
    uint64_t address = reinterpret_cast<uint64_t>(addressToFree);
    if(!currentHeap)
    {
        std::cerr<<"Can't find current heap\n";
        return NULL;
    }

    ++currentHeap;
    while(currentHeap)
    {
        uint64_t currentAddress = reinterpret_cast<uint64_t>(currentHeap);
        uint64_t lastAddressOfChunk =
            currentAddress + getCorrectSize(currentHeap->chunkSize) - 1;
        if(currentAddress <= address && address <= lastAddressOfChunk)
        {
            return currentHeap;
        }
        currentHeap = reinterpret_cast<Header*>(currentHeap->next);
    }
    return NULL;
 }

void FirstFitMemmoryAllocator::merge(Header* address)
{
    bool isFirstHeader = isPreviousHeaderFirst(address);
    if(isFirstHeader)
    {
        if(address->next == 0)
        {
            address->release();
        }
        else
        {
            Header* nextBlock = reinterpret_cast<Header*>(address->next);
            if(nextBlock->isFree())
            {
                address->next = nextBlock->next;
                address->chunkSize += nextBlock->chunkSize;
            }
            else
            {
                address->release();
            }
        }
        return;
    }

    else if(address->next == 0)
    {
         Header* prevBlock = reinterpret_cast<Header*>(address->prev);
         if(prevBlock->isFree())
         {
             prevBlock->next = address->next;
             prevBlock->chunkSize += address->chunkSize;
         }
         else
         {
            address->release();
         }
         return;
    }
    /*
     * 100% we have left and right NEIGHBOURS
     */
    Header* prevBlock = reinterpret_cast<Header*>(address->prev);
    Header* nextBlock = reinterpret_cast<Header*>(address->next);

    if(prevBlock->isFree())
    {
        if(nextBlock->isFree())
        {
            prevBlock->next = nextBlock->next;
            if(nextBlock->next != 0)
            {
                 Header* nextNextBlock = reinterpret_cast<Header*>(nextBlock->next);
                 nextNextBlock->prev = reinterpret_cast<uint64_t>(prevBlock);
            }
            prevBlock->chunkSize += address->chunkSize + nextBlock->chunkSize;
        }
        else
        {
            prevBlock->next = address->next;
            prevBlock->chunkSize += address->chunkSize;
        }
        return;
    }
    else
    {
        if(nextBlock->isFree())
        {
            address->next = nextBlock->next;
            address->chunkSize += nextBlock->chunkSize;
        }
        else
        {
            address->release();
        }
        return;
    }
}

bool FirstFitMemmoryAllocator::isPreviousHeaderFirst(Header* currentHeader)
{
    uint64_t distance = reinterpret_cast<uint64_t>(currentHeader) - currentHeader->prev;
    return (distance == 0 || distance == SIZE_HEADER) ? true : false;
}

void FirstFitMemmoryAllocator::ramStateInfo()
{
    size_t count_chunks = 0;
    uint64_t firstAddress, lastAddress;
    firstAddress = reinterpret_cast<uint64_t>(ram);
    Header* pool = reinterpret_cast<Header*>(ram);
    lastAddress = firstAddress + getCorrectSize(pool->chunkSize);
    if(!pool->isFree())
    {
        ++pool;
        while(pool)
        {
            std::cout << "Pool is free/occur: " << pool->isFree() << std::endl;
            std::cout << "Current address is: " << (uint64_t)pool << std::endl;
            std::cout << "Previous chunk is: " << pool->prev << std::endl;
            std::cout << "Next chunk is: " << pool->next << std::endl;
            std::cout << "Current chunk size is: " << getCorrectSize(pool->chunkSize) << std::endl;
            count_chunks++;
            pool = reinterpret_cast<Header*>(pool->next);
        }

    }

    else
    {
        std::cout << "The ram is empty:\n";
        std::cout << "First address is: " << firstAddress << std::endl
                  << "Last address is: " << lastAddress << std::endl;
        std::cout << "Size of the ram is: " << getCorrectSize(pool->chunkSize) << std::endl;
    }

    std::cout << "Number of blocks is: " << count_chunks << std::endl;
}


