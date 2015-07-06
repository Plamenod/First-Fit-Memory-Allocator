#include <iostream>
#include "FirstFitMemmoryAllocator.h"

int main()
{
    FirstFitMemmoryAllocator f;
    f.ramStateInfo();
    std::cout << "construct FFMA\n\n";

    void * p = f.allocateBlocks(10);
    uint64_t d = (uint64_t)p;
    f.ramStateInfo();
    std::cout << "allocate 10 size \n\n";
    void * k = f.allocateBlocks(10);
    f.ramStateInfo();
     std::cout << "allocate 10 size \n\n";
    f.release(p);
    f.ramStateInfo();
     std::cout << "release first block \n\n";
     f.release(k);
    f.ramStateInfo();
     std::cout << "release first block \n\n";

    return 0;
}
