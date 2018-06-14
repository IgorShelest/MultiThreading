#pragma once

#include <windows.h>
#include <string>

//-----------------------------------------------------------------------------

const unsigned int  g_buf_size               = 256;
const TCHAR         g_shared_memory_name[]   = TEXT("Global\\file_mapping_object");
const TCHAR         g_shared_io_mutex_name[] = TEXT("shared_mutex");
HANDLE              g_event_handle;
const std::string   g_log_file_name          = "log_file.txt";
