// 18_Multithreaded_Pipe_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <strsafe.h>
#include <string>

//-----------------------------------------------------------------------------

const unsigned    g_buffer_size = 512;
const std::string g_pipe_name = "\\\\.\\pipe\\mynamedpipe";

//-----------------------------------------------------------------------------

// This routine is a simple function to print the client request to the console
// and populate the reply buffer with a default data string. This is where you
// would put the actual client request processing code that runs in the context
// of an instance thread. Keep in mind the main thread will continue to wait for
// and receive other client connections while the instance thread is working.

VOID GetAnswerToRequest(LPTSTR request_buffer_input, LPTSTR reply_buffer_input, LPDWORD bytes_replied_num_input)
{
    if (UNICODE)
    {
        std::wstring buffer_input_wstr = (wchar_t*)request_buffer_input;
        std::string buffer_input_str(buffer_input_wstr.begin(), buffer_input_wstr.end());
        std::cout << "\nClient Request String: " << buffer_input_str;
    }
    else
    {
        std::cout << "\nClient Request String: " << std::string((char*)request_buffer_input);
    }

    

    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy(reply_buffer_input, g_buffer_size, TEXT("default answer from server"))))
    {
        *bytes_replied_num_input = 0;
        reply_buffer_input[0] = 0;
                
        std::cout << "\nStringCchCopy failed, no outgoing message.";
        return;
    }

    *bytes_replied_num_input = (lstrlen(reply_buffer_input) + 1) * sizeof(TCHAR);
}

//-----------------------------------------------------------------------------

// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.

DWORD WINAPI InstanceThread(LPVOID pipe_handle_input)
{
    // Check incoming thread parameter
    if (pipe_handle_input == NULL)
    {
        std::cout << "\nPipe Server::InstanceThread: ERROR - Pipe Server Failure: \n"
                  << "   InstanceThread got an unexpected NULL value in lpvParam.\n"
                  << "   InstanceThread exiting.";

        return (DWORD)-1;
    }

    HANDLE heap_handle = GetProcessHeap();
    TCHAR* request_buffer = (TCHAR*)HeapAlloc(heap_handle, 0, g_buffer_size * sizeof(TCHAR));
    TCHAR* reply_buffer = (TCHAR*)HeapAlloc(heap_handle, 0, g_buffer_size * sizeof(TCHAR));

    if (request_buffer == NULL || reply_buffer == NULL)
    {
        std::cout << "\nPipe Server::InstanceThread: ERROR - Pipe Server Failure: \n"
            << "   InstanceThread got an unexpected NULL heap allocation.\n"
            << "   InstanceThread exiting.";
        
        if (request_buffer != NULL) HeapFree(heap_handle, 0, request_buffer);
        if (reply_buffer != NULL) HeapFree(heap_handle, 0, reply_buffer);

        return (DWORD)-1;
    }

    std::cout << "\nPipe Server::Instance Thread: InstanceThread created, receiving and processing messages.";

    HANDLE pipe_handle = (HANDLE)pipe_handle_input;

    // Loop until everything is read from the pipe 
    while (true)
    {
        BOOL file_processed = FALSE;
        DWORD bytes_read_num = 0, bytes_replied_num = 0, bytes_written_num = 0;

        // Read client requests from the pipe up to g_buffer_size characters in length
        file_processed = ReadFile(
            pipe_handle,                    // handle to pipe 
            request_buffer,                 // buffer to receive data 
            g_buffer_size * sizeof(TCHAR),  // size of buffer 
            &bytes_read_num,                // number of bytes read 
            NULL);                          // not overlapped I/O 

        if (!file_processed || bytes_read_num == 0)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
            {
                std::cout << "\nInstanceThread: client disconnected." << GetLastError();
            }
            else
            {
                std::cout << "\nInstanceThread: ReadFile failed. ERROR: " << GetLastError();
            }
            break;
        }

        // Process the incoming message.
        GetAnswerToRequest(request_buffer, reply_buffer, &bytes_replied_num);

        // Write the reply to the pipe. 
        file_processed = WriteFile(
            pipe_handle,        // handle to pipe 
            reply_buffer,       // buffer to write from 
            bytes_replied_num,  // number of bytes to write 
            &bytes_written_num, // number of bytes written 
            NULL);              // not overlapped I/O 

        if (!file_processed || bytes_replied_num != bytes_written_num)
        {
            std::cout << "\nInstance Thread: WriteFile failed. ERROR: " << GetLastError();
            break;
        }
    }

    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting. Then disconnect the pipe, and close the 
    // handle to this pipe instance. 
    FlushFileBuffers(pipe_handle);
    DisconnectNamedPipe(pipe_handle);
    CloseHandle(pipe_handle);

    HeapFree(heap_handle, 0, request_buffer);
    HeapFree(heap_handle, 0, reply_buffer);

    std::cout << "\nInstanceThread exiting.\n";
    return 1;
}

//-----------------------------------------------------------------------------

int main()
{
    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.
    for (;;)
    {
        std::cout << "\nPipe Server::Main thread: Waiting for client to connect to: " << g_pipe_name ;

        HANDLE pipe_handle = INVALID_HANDLE_VALUE;

        // Convert Pipe Name
        TCHAR* pipe_name = new TCHAR[g_pipe_name.size() + 1];
        copy(g_pipe_name.begin(), g_pipe_name.end(), pipe_name);   // -D_SCL_SECURE_NO_WARNINGS
        pipe_name[g_pipe_name.size()] = 0;

        pipe_handle = CreateNamedPipe(
            pipe_name,                // pipe name 
            PIPE_ACCESS_DUPLEX,       // read/write access 
            PIPE_TYPE_MESSAGE |       // message type pipe 
            PIPE_READMODE_MESSAGE |   // message-read mode 
            PIPE_WAIT,                // blocking mode 
            PIPE_UNLIMITED_INSTANCES, // max. instances  
            g_buffer_size,            // output buffer size 
            g_buffer_size,            // input buffer size 
            0,                        // client time-out 
            NULL);                    // default security attribute 

        if (pipe_handle == INVALID_HANDLE_VALUE)
        {
            std::cout << "\nPipe Server::Main thread: CreateNamedPipe failed. ERROR: " << GetLastError();
            return -1;
        }

        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
        BOOL pipe_is_connected = ConnectNamedPipe(pipe_handle, NULL) ?
            TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if(pipe_is_connected)
        {
            // Create a thread for this client. 
            HANDLE thread_handle = CreateThread(
                NULL,                   // no security attribute 
                0,                      // default stack size 
                InstanceThread,         // thread proc
                (LPVOID)pipe_handle,    // thread parameter 
                0,                      // not suspended 
                NULL);

            if (thread_handle == NULL)
            {
                std::cout << "\nPipe Server::Main thread: CreateThread failed. ERROR: " << GetLastError();
                return -1;
            }
            else CloseHandle(thread_handle);
        }
        else
        {
            // The client could not connect, so close the pipe. 
            CloseHandle(pipe_handle);
        }
    }

    std::system("pause");
    return 0;
}

