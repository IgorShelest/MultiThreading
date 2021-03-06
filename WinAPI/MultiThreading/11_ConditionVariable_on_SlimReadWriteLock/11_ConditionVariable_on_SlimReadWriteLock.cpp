// 11_ConditionVariable_on_SlimReadWriteLock.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iostream>
#include <iomanip>

//-----------------------------------------------------------------------------

SRWLOCK             g_srw_lock;
CONDITION_VARIABLE  g_condition_variable_incremented;
CONDITION_VARIABLE  g_condition_variable_decremented;

//-----------------------------------------------------------------------------

unsigned int        g_data = 0;
bool                g_stop_requested = false;

//-----------------------------------------------------------------------------

DWORD WINAPI myThreadIncrementer(LPVOID lpParameter)
{
    while (!g_stop_requested || (g_stop_requested && (g_data % 5 != 0)))
    {
        AcquireSRWLockExclusive(&g_srw_lock);

        while (g_data > 5)
            SleepConditionVariableSRW(&g_condition_variable_decremented, &g_srw_lock, INFINITE, 0);

        ++g_data;

        ReleaseSRWLockExclusive(&g_srw_lock);

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
    while (!g_stop_requested || (g_stop_requested && (g_data != 0)))
    {
        AcquireSRWLockExclusive(&g_srw_lock);

        while (g_data < 5)
            SleepConditionVariableSRW(&g_condition_variable_incremented, &g_srw_lock, INFINITE, 0);

        g_data -= 5;

        ReleaseSRWLockExclusive(&g_srw_lock);

        WakeConditionVariable(&g_condition_variable_decremented);
    }

    return 0;
}

//-----------------------------------------------------------------------------

DWORD WINAPI myThreadReader(LPVOID lpParameter)
{
    while (!g_stop_requested || (g_stop_requested && (g_data != 0)))
    {
        AcquireSRWLockShared(&g_srw_lock);

        std::cout << "\nRead data = " << g_data;

        ReleaseSRWLockShared(&g_srw_lock);

        Sleep(1000);
    }

    return 0;
}

//-----------------------------------------------------------------------------

int main()
{
    InitializeConditionVariable(&g_condition_variable_incremented);
    InitializeConditionVariable(&g_condition_variable_decremented);

    InitializeSRWLock(&g_srw_lock);

    unsigned int error_count = 0;

    for (unsigned int iter = 0; iter < 10; ++iter)
    {
        HANDLE thread_array[3];
        thread_array[0] = CreateThread(0, 0, myThreadIncrementer, 0, 0, 0);
        thread_array[1] = CreateThread(0, 0, myThreadDecrementer, 0, 0, 0);
        thread_array[2] = CreateThread(0, 0, myThreadReader, 0, 0, 0);

        // Let child threads do some work
        Sleep(1000);

        // Trigger child threads to exit
        g_stop_requested = true;

        WaitForMultipleObjects(3, thread_array, true, INFINITE);

        CloseHandle(thread_array[0]);
        CloseHandle(thread_array[1]);
        CloseHandle(thread_array[2]);

        if (g_data != 0)
        {
            std::cout << "\nResult Data = " << g_data;
            ++error_count;
        }

        g_data = 0;
        g_stop_requested = false;
    }

    std::cout << "\nNum of errors: " << error_count;

    system("pause");
    return 0;
}