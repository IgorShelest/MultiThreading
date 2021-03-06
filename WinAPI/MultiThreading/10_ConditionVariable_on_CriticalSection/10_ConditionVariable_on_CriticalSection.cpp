// 10_ConditionVariable_on_CriticalSection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------

CRITICAL_SECTION    g_critical_section;
CONDITION_VARIABLE  g_condition_variable_incremented;
CONDITION_VARIABLE  g_condition_variable_decremented;

//-----------------------------------------------------------------------------

unsigned int        g_data = 0;
bool                g_stop_requested = false;

//-----------------------------------------------------------------------------

DWORD WINAPI myThreadIncremeneter(LPVOID lpParameter)
{
    while ( !g_stop_requested || ( g_stop_requested && (g_data % 5 != 0) ) )
    {
        EnterCriticalSection(&g_critical_section);

        while (g_data > 5)
            SleepConditionVariableCS(&g_condition_variable_decremented, &g_critical_section, INFINITE);

        ++g_data;

        LeaveCriticalSection(&g_critical_section);

        if ((g_data % 5) == 0)
        {
            WakeConditionVariable(&g_condition_variable_incremented);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------

DWORD WINAPI myThreadDecrementer(LPVOID lpParameter)
{
    while ( !g_stop_requested || (g_stop_requested && (g_data != 0)) )
    {
        EnterCriticalSection(&g_critical_section);

        while (g_data < 5)
            SleepConditionVariableCS(&g_condition_variable_incremented, &g_critical_section, INFINITE);

        g_data -= 5;

        LeaveCriticalSection(&g_critical_section);

        WakeConditionVariable(&g_condition_variable_decremented);
    }

    return 0;
}

//-----------------------------------------------------------------------------

int main()
{
    InitializeCriticalSection(&g_critical_section);
    InitializeConditionVariable(&g_condition_variable_incremented);
    InitializeConditionVariable(&g_condition_variable_decremented);
    unsigned int error_count = 0;

    for (unsigned int iter = 0; iter < 10; ++iter)
    {
        HANDLE thread_array[2];
        thread_array[0] = CreateThread(0, 0, myThreadIncremeneter, 0, 0, 0);
        thread_array[1] = CreateThread(0, 0, myThreadDecrementer, 0, 0, 0);

        // Let child threads do some work
        Sleep(1000);

        // Trigger child threads to exit
        g_stop_requested = true;

        WaitForMultipleObjects(2, thread_array, true, INFINITE);

        CloseHandle(thread_array[0]);
        CloseHandle(thread_array[1]);

        if (g_data != 0)
        {
            std::cout << "\nData = " << g_data;
            ++error_count;
        }

        g_stop_requested = false;
        g_data = 0;
    }

    std::cout << "\nNum of errors: " << error_count;

    DeleteCriticalSection(&g_critical_section);

    system("pause");
    return 0;
}