// 08_1_Named_Mutex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------

static const std::string g_mutex_name = "SharedMutex";

//-----------------------------------------------------------------------------

DWORD WINAPI thread_function(LPVOID lpParameter)
{
    // Convert mutex name
    LPCSTR named_mutex_name = g_mutex_name.c_str();

    // Open named Mutex
    HANDLE named_mutex_handle = OpenMutexA(
        MUTEX_ALL_ACCESS,   // Desired Access
        TRUE,               // Inherit Handle 
        named_mutex_name);  // Event Name

    if (!named_mutex_handle)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
            std::cout << "\nLast ERROR: " << GetLastError();
        else
            std::cout << "\nLast ERROR: " << "XZ";

        return -1;
    }

    for (unsigned int iter = 0; iter < 20; ++iter)
    {
        // Waiting infinitely for the mutex to unlock
        WaitForSingleObject(named_mutex_handle, INFINITE);

        std::cout << "\nThread ID: " << GetCurrentThreadId()
            << " thread_function executing smth.";

        ReleaseMutex(named_mutex_handle);
    }

    CloseHandle(named_mutex_handle);

    return 0;
}

//-----------------------------------------------------------------------------

int main()
{
    // Convert mutex name
    LPCSTR named_mutex_name = g_mutex_name.c_str();
    HANDLE mutex_handle = CreateMutexA(0, 0, named_mutex_name);

    HANDLE thread_array[2];
    thread_array[0] = CreateThread(0, 0, thread_function, 0, 0, 0);
    thread_array[1] = CreateThread(0, 0, thread_function, 0, 0, 0);

    WaitForMultipleObjects(2, thread_array, true, INFINITE);

    CloseHandle(thread_array[0]);
    CloseHandle(thread_array[1]);

    CloseHandle(mutex_handle);

    system("pause");
    return 0;
}