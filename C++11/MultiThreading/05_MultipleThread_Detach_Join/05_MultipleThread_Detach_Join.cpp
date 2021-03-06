// 05_MultipleThread_Detach_Join.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <thread>
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <vector>
#include <algorithm>

//-----------------------------------------------------------------------------

void thread_function()
{
    for (unsigned iter = 0; iter < 10; ++iter)
    {
        std::cout << "\nThread: " << std::this_thread::get_id() << " is doing some work";
    }
}

//-----------------------------------------------------------------------------

int main()
{
    std::cout << "\nMain thread START";

    // Launching threads
    std::vector<std::thread> threads;
    for (unsigned iter = 0; iter < 10; ++iter)
    {
        threads.push_back(std::thread(thread_function));
    }
    
    // Let the threads do some work
    Sleep(1);

    // Join all the threads
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    std::cout << "\nMain thread END\n";
    std::system("pause");
    return 0;
}
