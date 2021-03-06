// 08_Assighne_Pointer_to_member_function_as_thread_function.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <thread>
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>
#include <sstream>

//-----------------------------------------------------------------------------

class SomeObject
{
public:
    void member_function_1()
    {
        for (unsigned iter = 0; iter < 100; ++iter)
        {
            std::cout << "\nmember_function_1: " << std::this_thread::get_id() << " is doing some work";
        }
    }
    void member_function_2(int counter, std::string data)
    {
        for (unsigned iter = 0; iter < counter; ++iter)
        {
            std::cout << "\nmember_function_2: " << std::this_thread::get_id() << " is working on data: " << data << "\n";
        }
    }
};

//-----------------------------------------------------------------------------

int main()
{
    std::cout << "\nMain thread START";

    const int counter = 10;
    const std::string data = "Some data";

    // Object must be initialized to use it's member functions
    SomeObject object;

    std::thread thread_1(&SomeObject::member_function_1, &object);
    std::thread thread_2(&SomeObject::member_function_2, &object, counter, data);

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
