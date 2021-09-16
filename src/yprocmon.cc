/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yprocmon.cc
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief yprocmon 主模块
 */

#include <future>
#include <iostream>
#include <shlwapi.h>
#include <vector>
#include <windows.h>

#include "debug.h"
#include "detours.h"
#include "yhook.h"
#include "yhttp.h"
#include "yipcsrv.h"
#include "yprocmon.h"
#include "ystr.h"

std::vector<file_entry> get_files()
{
    std::vector<file_entry> v;
    file_entry result;
    WIN32_FIND_DATA file;
    SYSTEMTIME time;
    TCHAR dir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, dir);
    PathAppend(dir, TEXT(SAMPLE_DIR));
    PathAppend(dir, TEXT("\\*"));
    console_print("[HTTP] Listing files at %s.\n", tostring(dir).c_str());
    HANDLE hFile = FindFirstFile(dir, &file);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                FileTimeToSystemTime(&file.ftCreationTime, &time);
                v.push_back(file_entry{tostring(file.cFileName), jstime(time),
                                       file.nFileSizeLow});
            }
        } while (FindNextFile(hFile, &file) != 0);
    }
    return v;
}

int _detour_program(const PROCESS_INFORMATION pi)
{
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD dwResult = 0;
    if (!GetExitCodeProcess(pi.hProcess, &dwResult))
    {
        console_print("[PROC] GetExitCodeProcess failed: %ld\n",
                      GetLastError());
    }
    std::lock_guard<std::mutex> guard(state.instances_mutex);
    if (state.instances.count(pi.dwProcessId) != 0)
    {
        state.instances[pi.dwProcessId].status = INSTANCE_EXITED;
        state.instances[pi.dwProcessId].exitCode = dwResult;
        state.instances[pi.dwProcessId].waiting.detach();
    }
    return 0;
}

bool detour_program(const detour_startup& startup, instance_entry& ret)
{
    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    TCHAR path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);
    PathAppend(path, TEXT(SAMPLE_DIR));
    PathAppend(path, towstring(startup.name).c_str());

    TCHAR dll_path[MAX_PATH];
    OutputDebugStringW(dll_path);
    GetCurrentDirectory(MAX_PATH, dll_path);
    OutputDebugStringW(dll_path);
    PathAppend(dll_path, TEXT(YHOOK_DLL));
    OutputDebugStringW(dll_path);
    std::string dll_path_str = tostring(dll_path);
    LPCSTR dll = dll_path_str.c_str();

    console_print("[PROC] Detouring program... path = %s, command = %s, dll = %s\n",
                  tostring(path).c_str(), startup.command.c_str(), dll);
    if (!DetourCreateProcessWithDlls(
        path, (LPWSTR)towstring(startup.command).c_str(), NULL, NULL, TRUE,
        dwFlags, NULL, NULL, &si, &pi, 1, &dll, NULL))
    {
        DWORD dwError = GetLastError();
        console_print("[PROC] "
                      "DetourCreateProcessWithDllEx failed: %ld\n",
                      dwError);
        if (dwError == ERROR_INVALID_HANDLE)
        {
            console_print("[PROC] Can't detour a mixed target process.\n");
        }
        return false;
    }
    console_print("[PROC] Start a thread waiting for pid %u.\n", pi.dwProcessId);
    ret.waiting = std::thread (_detour_program, pi);
    ret.pi.dwProcessId = pi.dwProcessId;
    ret.pi.dwThreadId = pi.dwThreadId;
    ret.pi.hProcess = pi.hProcess;
    ret.pi.hThread = pi.hThread;
    ret.status = INSTANCE_SPAWNED;
    ret.name = std::string(startup.name);
    ret.timestamp = time(nullptr) * 1000;
    return true;
}

int main(int argc, char **argv)
{
    state.port = YPROCMON_HTTP_PORT;
    _wmkdir(TEXT(SAMPLE_DIR));
    console_print("[HTTP] "
                  "Starting HTTP server at 0.0.0.0:%d.\n",
                  state.port);
    std::thread thread_http(start_http_server);
    console_print("[IPC] "
                  "Initializing IPC pipe at %s.\n",
                  IPC_PIPE);
    std::thread thread_ipc(start_ipc_server);

    // Sleep(2000);
    // console_print("[INFO] " "Starting browser at %s.\n", YPROCMON_HTTP_URL);
    // ShellExecute(NULL, TEXT("open"), TEXT(YPROCMON_HTTP_URL), NULL, NULL,
    // SW_SHOWNORMAL);

    thread_http.join();
    thread_ipc.join();
    return 0;
}