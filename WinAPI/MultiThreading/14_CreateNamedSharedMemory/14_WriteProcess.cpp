// WriteProcess.cpp : Defines the entry point for the console application.
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
//                        NULL,               // default security attributes
//                        TRUE,               // manual-reset event
//                        FALSE,              // initial state is nonsignaled
//                        TEXT("WriteEvent")  // object name
//                    );
//
//    WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//    g_log_file.open(g_log_file_name, std::ios::app);
//    g_log_file << "\nWriteProcess::Starting " << argv[0];
//    g_log_file.close();
//    ReleaseMutex(&g_shared_io_mutex_handle);
//
//    // Create File Mapping object
//    HANDLE file_mapping_handle = CreateFileMapping(
//                                    INVALID_HANDLE_VALUE,    // use paging file
//                                    NULL,                    // default security
//                                    PAGE_READWRITE,          // read/write access
//                                    0,                       // maximum object size (high-order DWORD)
//                                    g_buf_size,              // maximum object size (low-order DWORD)
//                                    g_shared_memory_name);   // name of mapping object
//
//    if (file_mapping_handle == NULL)
//    {
//        WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//        g_log_file.open(g_log_file_name, std::ios::app);
//        g_log_file << "\nWriteProcess::Could not create file mapping object: " << GetLastError();
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
//                                            file_mapping_handle,    // handle to map object
//                                            FILE_MAP_ALL_ACCESS,    // read/write permission
//                                            0,
//                                            0,
//                                            g_buf_size);
//
//    if (file_mapping_view == NULL)
//    {
//        WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//        g_log_file.open(g_log_file_name, std::ios::app);
//        g_log_file << "\nWriteProcess::Could not get file mapping view: " << GetLastError();
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
//    // Create data to share
//    TCHAR   shared_memory_data[] = TEXT("Message from first process.");
//
//    // Copy data into File Mapping View
//    CopyMemory((PVOID)file_mapping_view, shared_memory_data, (_tcslen(shared_memory_data) * sizeof(TCHAR)));
//
//    // Make Read Process wait for event
//    Sleep(3000);
//
//    // Notify Read Process that shared data is available
//    SetEvent(g_event_handle);
//
//    WaitForSingleObject(&g_shared_io_mutex_handle, INFINITE);
//    g_log_file.open(g_log_file_name, std::ios::app);
//    g_log_file << "\nWriteProcess::Ending";
//    g_log_file.close();
//    ReleaseMutex(&g_shared_io_mutex_handle);
//
//    UnmapViewOfFile(file_mapping_view);
//    CloseHandle(file_mapping_handle);
//    CloseHandle(g_event_handle);
//    CloseHandle(g_shared_io_mutex_handle);
//
//    return 0;
//}