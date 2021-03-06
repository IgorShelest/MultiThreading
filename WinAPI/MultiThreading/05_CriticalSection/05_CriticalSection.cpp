// 05_CriticalSection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------

CRITICAL_SECTION g_critical_section;

//-----------------------------------------------------------------------------

DWORD WINAPI thread_function(LPVOID lpParameter)
{
    for (unsigned int iter = 0; iter < 20; ++iter)
    {
        // Waiting infinitely for the Critical Section to unlock
        EnterCriticalSection(&g_critical_section);

        std::cout << "\nThread ID: " << GetCurrentThreadId()
            << " thread_function executing smth.";

        LeaveCriticalSection(&g_critical_section);
    }

    return 0;
}

//-----------------------------------------------------------------------------

int main()
{
    InitializeCriticalSection(&g_critical_section);

    HANDLE thread_array[2];
    thread_array[0] = CreateThread(0, 0, thread_function, 0, 0, 0);
    thread_array[1] = CreateThread(0, 0, thread_function, 0, 0, 0);

    WaitForMultipleObjects(2, thread_array, true, INFINITE);

    CloseHandle(thread_array[0]);
    CloseHandle(thread_array[1]);

    DeleteCriticalSection(&g_critical_section);

    system("pause");
    return 0;
}