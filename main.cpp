#include <iostream>
#include "SafeQueue.h"

int main ()
{
    std::cout << "Hello, World!" << std::endl;


    SafeQueue<int> safeQueue (5);

    safeQueue.push (1);
    safeQueue.push (2);
    safeQueue.push (3);
    safeQueue.push (4);


    int popped = 0;

    while (!safeQueue.empty ())
    {
        safeQueue.pop (popped);
        std::cout << popped << std::endl;
    }


    return 0;
}