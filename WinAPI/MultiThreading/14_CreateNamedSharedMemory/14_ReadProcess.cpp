// ReadProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iomanip>
#include <iostream>
#include <fstream>

//-----------------------------------------------------------------------------

//#include "14_SharedProcessData.h"
//
////-----------------------------------------------------------------------------
//
//HANDLE          g_shared_io_mutex_handle;
//std::ofstream   g_log_file;
//
////-----------------------------------------------------------------------------
//
//int main(int argc, char* argv[])
//{
//    // Get handle to the shared mutex
//    g_shared_io_mutex_handle = CreateMutex(0, 0, g_shared_io_mutex_name);
//
//    // Create Event
//    g_event_handle = CreateEvent(
//        NULL,               // default security attributes
//        TRUE,               // manual-reset event
//        FALSE,              // initial state is nonsignaled
//        TEXT("WriteEvent")  // object name
//    );
//
//    WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//    g_log_file.open(g_log_file_name, std::ios::app);
//    g_log_file << "\nReadProcess::Starting " << argv[0];
//    g_log_file.close();
//    ReleaseMutex(&g_shared_io_mutex_handle);
//    
//    // Open previously created File Mapping object
//    HANDLE file_mapping_handle = OpenFileMapping(
//                                    FILE_MAP_ALL_ACCESS,   // read/write access
//                                    FALSE,                 // do not inherit the name
//                                    g_shared_memory_name); // name of mapping object
//
//    if (file_mapping_handle == NULL)
//    {
//        WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//        g_log_file.open(g_log_file_name, std::ios::app);
//        g_log_file << "\nReadProcess::Could not create file mapping object: " << GetLastError();
//        g_log_file.close();
//        ReleaseMutex(&g_shared_io_mutex_handle);
//
//        CloseHandle(g_shared_io_mutex_handle); 
//        CloseHandle(g_event_handle);
//
//        return 1;
//    }
//
//    // Get File Mapping View
//    LPCTSTR file_mapping_view = (LPTSTR) MapViewOfFile(
//                                    file_mapping_handle,    // handle to map object
//                                    FILE_MAP_ALL_ACCESS,    // read/write permission
//                                    0,
//                                    0,
//                                    g_buf_size);
//
//    if (file_mapping_view == NULL)
//    {
//        WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//        g_log_file.open(g_log_file_name, std::ios::app);
//        g_log_file << "\nReadProcess::Could not get file mapping view: " << GetLastError();
//        g_log_file.close();
//        ReleaseMutex(&g_shared_io_mutex_handle);
//
//        CloseHandle(file_mapping_handle);
//        CloseHandle(g_shared_io_mutex_handle);
//        CloseHandle(g_event_handle);
//
//        return 1;
//    }
//
//    // Wait until Write Process notifies that shared data is available
//    while (WaitForSingleObject(g_event_handle, 1000) == WAIT_TIMEOUT)
//    {
//        WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//        g_log_file.open(g_log_file_name, std::ios::app);
//        g_log_file << "\nReadProcess::doing some work while not signaled";
//        g_log_file.close();
//        ReleaseMutex(&g_shared_io_mutex_handle);
//    }
//
//    // Read shared data
//    MessageBox(NULL, file_mapping_view, TEXT("Process2"), MB_OK);
//    
//
//
//    WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//    g_log_file.open(g_log_file_name, std::ios::app);
//    g_log_file << "\nReadProcess::Ending";
//    g_log_file.close();
//    ReleaseMutex(&g_shared_io_mutex_handle);
//
//    UnmapViewOfFile(file_mapping_view);
//    CloseHandle(file_mapping_handle);
//    CloseHandle(g_shared_io_mutex_handle);
//    CloseHandle(g_event_handle);
//
//    return 0;
//}