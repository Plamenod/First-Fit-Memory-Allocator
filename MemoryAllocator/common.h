#include <stdint.h>

const size_t SIZE_OF_RAM = 1073741824;
const size_t SIZE_HEADER = 24;
const size_t START_OF_HEAP = 24;

/*
 * @breaf: helper struct which contents info about current chunk
 * @Note: Also every ram page will start with info about the new ram page,
 * and after that every chunk will start with the same info header.
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
    }

    bool isFree()
    {
        return chunkSize & (1 << 31);
    }

    void release()
    {
        chunkSize |= (1 << 31);
    }

    uint64_t prev;
    uint64_t next;
    uint32_t chunkSize;
};