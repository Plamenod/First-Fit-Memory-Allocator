#include <iostream>
#include "FirstFitMemmoryAllocator.h"

int main()
{
    FirstFitMemmoryAllocator f;
    f.ramStateInfo();
    std::cout << "construct FFMA\n\n";

    void * p = f.myMalloc(10);
    uint64_t d = (uint64_t)p;
    f.ramStateInfo();
    std::cout << "allocate 10 size \n\n";
    void * k = f.myMalloc(10);
    f.ramStateInfo();
     std::cout << "allocate 10 size \n\n";
    f.myFree(p);
    f.ramStateInfo();
     std::cout << "myFree first block \n\n";
     f.myFree(k);
    f.ramStateInfo();
     std::cout << "myFree first block \n\n";
    return 0;
}
