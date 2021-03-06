// 01_CreateThread_FunctionObject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <thread>
#include <iostream>
#include <windows.h>
#include <iomanip>

//-----------------------------------------------------------------------------

class ThreadObject
{
public:
    void operator()()
    {
        for (unsigned iter = 0; iter < 100; ++iter)
        {
            std::cout << "\nThread: " << std::this_thread::get_id() << " is doing some work";
        }
    }
};

//-----------------------------------------------------------------------------

int main()
{
    std::cout << "\nMain thread START";

    std::thread thread_1( (ThreadObject()) );
    std::thread thread_2( (ThreadObject()) );

    // Let the thread do some work
    Sleep(1);

    // Detaching first thread
    if (thread_1.joinable())
        thread_1.detach();

    // Waiting for the first thread to finish
    if (thread_2.joinable())
        thread_2.join();

    std::cout << "\nMain thread END\n";

    std::system("pause");
    return 0;
}
