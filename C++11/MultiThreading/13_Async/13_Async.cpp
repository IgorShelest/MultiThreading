// 13_Async.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>

//-----------------------------------------------------------------------------

#include <thread>
#include <mutex>
#include <future>

//-----------------------------------------------------------------------------

std::mutex g_io_mutex;

//-----------------------------------------------------------------------------

std::string thread_function(std::string data)
{
    Sleep(1000);

    std::cout << "\nThread " << std::this_thread::get_id() << " started processing data: " << data;
    return data + "_processed";
}

//-----------------------------------------------------------------------------

int main()
{
    std::cout << "\nMain thread START";

    std::future<std::string> future = std::async(std::launch::async, thread_function, "Data");

    // Wait until data is processed in another thread
    std::cout << "\nMain thread waits for data to be set until it gets: " << future.get();

    std::cout << "\nMain thread END\n";

    std::system("pause");
    return 0;
}
