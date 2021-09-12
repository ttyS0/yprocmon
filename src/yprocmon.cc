/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yprocmon.cc
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief yprocmon 主模块
 */

#include <iostream>
#include <vector>
#include <windows.h>
#include <detours.h>

#include "yhook.h"
#include "yprocmon.h"

#include "yhttp.h"
#include "yipcsrv.h"

#include "debug.h"

yprocmon_state state;

// std::vector<yprocmon_thread>& threads = state.threads;

// DWORD create_thread(LPTHREAD_START_ROUTINE func, LPVOID param)
// {
//     HANDLE hThread;
//     DWORD threadID;
//     hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, param, 0,
//     &threadID); threads.push_back({ hThread, threadID }); return threadID;
// }

// void clean_thread()
// {
//     for(const auto& t : threads)
//     {
//         CloseHandle(t.handle);
//     }
// }

int detour_program()
{
    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    DWORD nDlls = 1;
    LPCSTR rpszDllsOut[256];
    rpszDllsOut[0] = "yhook.dll";
    DWORD error = DetourCreateProcessWithDlls(
        TEXT("C:\\Users\\scalecom\\Desktop\\undegraduate\\fresh-a\\software-"
             "security-project\\ymsgbox.exe"),
        TEXT("ymsgbox.exe"), NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi,
        nDlls, rpszDllsOut, NULL);
    if (!error)
    {
        DWORD dwError = GetLastError();
        console_print("[PROC] "
                      "DetourCreateProcessWithDllEx failed: %ld\n",
                      dwError);
        if (dwError == ERROR_INVALID_HANDLE)
        {
#if DETOURS_64BIT
            console_print(
                "[PROC] Can't detour a 32-bit target process from a 64-bit "
                "parent process.\n");
#else
            console_print(
                "[PROC] Can't detour a 64-bit target process from a 32-bit "
                "parent process.\n");
#endif
        }
        ExitProcess(9009);
    }

    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD dwResult = 0;
    if (!GetExitCodeProcess(pi.hProcess, &dwResult))
    {
        console_print("[PROC] GetExitCodeProcess failed: %ld\n",
                      GetLastError());
        return 9010;
    }
}

int main(int argc, char **argv)
{
    state.port = YPROCMON_HTTP_PORT;
    _wmkdir(TEXT("samples"));
    console_print("[HTTP] "
                  "Starting HTTP server at 0.0.0.0:%d.\n",
                  state.port);
    std::thread thread_http(start_http_server, state);
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