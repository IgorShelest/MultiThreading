// 12_1_Named_Event.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iomanip>
#include <iostream>
#include <algorithm>  

//-----------------------------------------------------------------------------

const unsigned int g_thread_amount = 2;
const static std::string g_event_name = "WriteEvent";

//-----------------------------------------------------------------------------

HANDLE g_thread_array[g_thread_amount];
HANDLE g_io_mutex_handle;

//-----------------------------------------------------------------------------

DWORD WINAPI wait_thread_function(LPVOID)
{
    WaitForSingleObject(g_io_mutex_handle, INFINITE);
    std::cout << "\nThread ID: " << GetCurrentThreadId() << " starting INFINITE WAIT";
    ReleaseMutex(g_io_mutex_handle);

    // Convert event name
    LPCSTR named_event_name = g_event_name.c_str();
    HANDLE named_event_handle = OpenEventA(EVENT_ALL_ACCESS,    // Desired Access
                                           TRUE,                // Inherit Handle 
                                           named_event_name);   // Event Name
    if (!named_event_handle)
    {
        WaitForSingleObject(g_io_mutex_handle, INFINITE);
        std::cout << "\nThread ID: " << GetCurrentThreadId() << " wrong named_event_handle";
        ReleaseMutex(g_io_mutex_handle);
        return 0;
    }

    WaitForSingleObject(named_event_handle, INFINITE);

    WaitForSingleObject(g_io_mutex_handle, INFINITE);
    std::cout << "\nThread ID: " << GetCurrentThreadId() << " signaled";
    ReleaseMutex(g_io_mutex_handle);

    CloseHandle(named_event_handle);

    return 0;
};

//-----------------------------------------------------------------------------

DWORD WINAPI do_thread_function(LPVOID)
{
    WaitForSingleObject(g_io_mutex_handle, INFINITE);
    std::cout << "\nThread ID: " << GetCurrentThreadId() << " starting 1sec WAIT";
    ReleaseMutex(g_io_mutex_handle);

    // Convert event name
    LPCSTR named_event_name = g_event_name.c_str();
    HANDLE named_event_handle = OpenEventA(EVENT_ALL_ACCESS,    // Desired Access
                                           TRUE,                // Inherit Handle 
                                           named_event_name);   // Event Name

    if (!named_event_handle)
    {
        WaitForSingleObject(g_io_mutex_handle, INFINITE);
        std::cout << "\nThread ID: " << GetCurrentThreadId() << " wrong named_event_handle";
        ReleaseMutex(g_io_mutex_handle);
        return 0;
    }

    while (WaitForSingleObject(named_event_handle, 1000) == WAIT_TIMEOUT)
    {
        WaitForSingleObject(g_io_mutex_handle, INFINITE);
        std::cout << "\nThread ID: " << GetCurrentThreadId() << " doing some work while not signaled";
        ReleaseMutex(g_io_mutex_handle);
    }

    WaitForSingleObject(g_io_mutex_handle, INFINITE);
    std::cout << "\nThread ID: " << GetCurrentThreadId() << " signaled";
    ReleaseMutex(g_io_mutex_handle);

    CloseHandle(named_event_handle);

    return 0;
};

//-----------------------------------------------------------------------------

int main()
{
    // Create mutex for io
    g_io_mutex_handle = CreateMutex(0, 0, 0);

    // Convert event name
    TCHAR* named_event_name = new TCHAR[g_event_name.size() + 1];
    copy(g_event_name.begin(), g_event_name.end(), named_event_name);   // -D_SCL_SECURE_NO_WARNINGS
    named_event_name[g_event_name.size()] = 0;

    // Create Event
    HANDLE event_handle = CreateEvent(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        named_event_name    // object name
    );

    // Start daughter threads
    g_thread_array[0] = CreateThread(0, 0, wait_thread_function, 0, 0, 0);
    g_thread_array[1] = CreateThread(0, 0, do_thread_function, 0, 0, 0);

    // Daughter threads will wait for the Event to be set
    Sleep(10000);

    // Set Event
    SetEvent(event_handle);

    WaitForMultipleObjects(2, g_thread_array, true, INFINITE);
    CloseHandle(g_thread_array[0]);
    CloseHandle(g_thread_array[1]);

    CloseHandle(event_handle);

    std::cout << "\n";
    system("pause");
    return 0;
}

