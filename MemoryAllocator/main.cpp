#include <iostream>
#include "FirstFitMemmoryAllocator.h"

int main()
{
    std::cout << "Hello, world\n";
    FirstFitMemmoryAllocator f;
    f.allocateBlocks(10);
    f.allocateBlocks(10);


    return 0;
}
