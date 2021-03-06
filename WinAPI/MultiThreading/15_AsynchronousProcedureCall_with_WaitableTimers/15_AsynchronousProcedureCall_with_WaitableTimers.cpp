// 15_AsynchronousProcedureCall_with_WaitableTimers.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iomanip>
#include <iostream>

//-----------------------------------------------------------------------------

struct MyData
{
    TCHAR* szText;
    DWORD  dwValue;
};

//-----------------------------------------------------------------------------

VOID CALLBACK timer_callback_function(
                LPVOID lpArg,              // Data value
                DWORD dwTimerLowValue,     // Timer low value
                DWORD dwTimerHighValue)    // Timer high value

{
    // Formal parameters not used in this example.
    UNREFERENCED_PARAMETER(dwTimerLowValue);
    UNREFERENCED_PARAMETER(dwTimerHighValue);

    MyData *pMyData = (MyData*) lpArg;

    std::cout << "\nMessage: " << pMyData->szText 
              << "\nValue: "   << pMyData->dwValue;
    MessageBeep(0);
}

//-----------------------------------------------------------------------------

int main()
{
    HANDLE timer_handle = CreateWaitableTimer(
                            NULL,                   // Default security attributes
                            FALSE,                  // Create auto-reset timer
                            TEXT("MyTimer"));       // Name of waitable timer

    if (timer_handle != NULL)
    {
        __try
        {
            // Create an integer that will be used to signal the timer 
            // 5 seconds from now.
            __int64 time = -5 * 10000000;
            
            // Copy the relative time into a LARGE_INTEGER
            LARGE_INTEGER l_time;
            l_time.LowPart = (DWORD)(time & 0xFFFFFFFF);
            l_time.HighPart = (LONG)(time >> 32);

            // Create Data
            MyData my_data;
            my_data.szText = TEXT("This is my data");
            my_data.dwValue = 100;

            bool set_timer_result = SetWaitableTimer(
                                        timer_handle,               // Handle to the timer object
                                        &l_time,                    // When timer will become signaled
                                        2000,                       // Periodic timer interval of 2 seconds
                                        timer_callback_function,    // Completion routine
                                        &my_data,                   // Argument to the completion routine
                                        FALSE);                     // Do not restore a suspended system

            if (set_timer_result)
            {
                for (; my_data.dwValue < 1000; my_data.dwValue += 100)
                {
                    SleepEx(
                        INFINITE,    // Wait forever
                        TRUE);       // Put thread in an alertable state
                }
            }
            else
            {
                std::cout << "SetWaitableTimer failed with error: " << GetLastError();
            }
        }
        __finally
        {
            CloseHandle(timer_handle);
        }
    }
    else
    {
        std::cout << "CreateWaitableTimer failed with error: " << GetLastError();
    }


    std::cout << "\n";
    system("pause");
    return 0;
}

