#include <iostream>
#include "FirstFitMemmoryAllocator.h"

int main()
{
    std::cout << "Hello, world\n";
    FirstFitMemmoryAllocator f;
    void * p = f.allocateBlocks(10);
    f.allocateBlocks(10);
    f.release(p);

    return 0;
}
