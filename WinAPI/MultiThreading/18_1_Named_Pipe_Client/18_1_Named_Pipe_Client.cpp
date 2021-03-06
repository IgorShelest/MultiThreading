// 18_1_Named_Pipe_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//-----------------------------------------------------------------------------

#include <windows.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <strsafe.h>
#include <conio.h>

//-----------------------------------------------------------------------------

const unsigned    g_buffer_size = 512;
const std::string g_pipe_name = "\\\\.\\pipe\\mynamedpipe";

//-----------------------------------------------------------------------------


int main()
{
    HANDLE pipe_handle;

    // Try to open a named pipe; wait for it, if necessary.
    while (true)
    {
        // Convert Pipe Name
        TCHAR* pipe_name = new TCHAR[g_pipe_name.size() + 1];
        copy(g_pipe_name.begin(), g_pipe_name.end(), pipe_name);   // -D_SCL_SECURE_NO_WARNINGS
        pipe_name[g_pipe_name.size()] = 0;

        pipe_handle = CreateFile(
            pipe_name,      // pipe name 
            GENERIC_READ |  // read and write access 
            GENERIC_WRITE,
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 

        // Break if the pipe handle is valid. 
        if (pipe_handle != INVALID_HANDLE_VALUE)
            break;

        // Exit if an error other than ERROR_PIPE_BUSY occurs. 
        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            std::cout << "\nCould not open pipe. ERROR: "  << GetLastError();

            std::system("pause");
            return -1;
        }

        // All pipe instances are busy, so wait for 20 seconds. 
        if (!WaitNamedPipe(pipe_name, 20000))
        {
            std::cout << "\nCould not open pipe: 20 second wait timed out.";

            std::system("pause");
            return -1;
        }
    }

    // The pipe connected; change to message-read mode. 
    DWORD pipe_mode = PIPE_READMODE_MESSAGE;
    BOOL set_pipe_state = SetNamedPipeHandleState(
        pipe_handle,    // pipe handle 
        &pipe_mode,     // new pipe mode 
        NULL,           // don't set maximum bytes 
        NULL);          // don't set maximum time 

    if (!set_pipe_state)
    {
        std::cout << "\nSetNamedPipeHandleState failed. ERROR: " << GetLastError();

        std::system("pause");
        return -1;
    }

    // Send a message to the pipe server
    LPTSTR message_to_send = TEXT("Default message from client.");
    DWORD bytes_to_send_num = (lstrlen(message_to_send) + 1) * sizeof(TCHAR);
    DWORD bytes_sent;

    if (UNICODE)
    {
        std::wstring message_to_send_wstr = (wchar_t*)message_to_send;
        std::string message_to_send_str(message_to_send_wstr.begin(), message_to_send_wstr.end());
        std::cout << "\nSending " << bytes_to_send_num << " byte message: " << message_to_send_str;
    }
    else
    {
        std::cout << "\nSending " << bytes_to_send_num << " byte message: " << std::string((char*)message_to_send);
    }      

    BOOL write_file = WriteFile(
        pipe_handle,        // pipe handle 
        message_to_send,    // message 
        bytes_to_send_num,  // message length 
        &bytes_sent,        // bytes written 
        NULL);              // not overlapped 

    if (!write_file || bytes_to_send_num != bytes_sent)
    {
        std::cout << "\nWriteFile to pipe failed. ERROR: " << GetLastError();

        std::system("pause");
        return -1;
    }

    std::cout << "\nMessage sent to server, receiving reply as follows:";

    BOOL read_file;
    do
    {
        TCHAR  read_buffer[g_buffer_size];
        DWORD bytes_read_num;

        // Read from the pipe 
        read_file = ReadFile(
            pipe_handle,                    // pipe handle 
            read_buffer,                    // buffer to receive reply 
            g_buffer_size * sizeof(TCHAR),  // size of buffer 
            &bytes_read_num,                // number of bytes read 
            NULL);                          // not overlapped 

        if (!read_file && GetLastError() != ERROR_MORE_DATA)
            break;

        if (UNICODE)
        {
            std::wstring read_buffer_wstr = (wchar_t*)read_buffer;
            std::string read_buffer_str(read_buffer_wstr.begin(), read_buffer_wstr.end());
            std::cout << "\nRead message: " << read_buffer_str;
        }
        else
        {
            std::cout << "\nRead message: " << read_buffer;
        }
        

    } while (!read_file);  // repeat loop if ERROR_MORE_DATA 

    if (!read_file)
    {
        std::cout << "\nReadFile from pipe failed. ERROR: " << GetLastError();

        std::system("pause");
        return -1;
    }

    std::cout << "\nEnd of message, press ENTER to terminate connection and exit";
    _getch();

    std::system("pause");
    return 0;
}

