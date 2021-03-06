// 06_PassingArguments_into_Thread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <thread>
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>

//-----------------------------------------------------------------------------

void thread_function(int counter, std::string data)
{
    for (unsigned iter = 0; iter < counter; ++iter)
    {
        std::cout << "\nThread: " << std::this_thread::get_id() << " is working on data: " << data;
    }
}

//-----------------------------------------------------------------------------

int main()
{
    std::cout << "\nMain thread START";

    const unsigned counter = 10;
    const std::string data = "Some Data";

    std::thread thread_1(thread_function, counter, data);
    std::thread thread_2(thread_function, counter, data);

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
