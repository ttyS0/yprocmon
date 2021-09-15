/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yhook.cc
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief yhook 模块主文件
 */

#include "yhook.h"
#include "yhookmap.h"
#include "yipc.h"
#include "ystr.h"
#include "ytime.h"
#include <detours.h>
#include <string>
#include <vector>
#include <windows.h>

extern HANDLE pipe;

extern yhook_entry yhook_map[];

DWORD write_pipe(const uint8_t *data, const size_t length)
{
    DWORD dwWritten;
    if (pipe != INVALID_HANDLE_VALUE)
    {
        WriteFile(pipe, data, length, &dwWritten, NULL);
    }
    return dwWritten;
}

DWORD write_pipe(const std::string &s)
{
    return write_pipe((const uint8_t *)s.c_str(), s.size());
}

void send_message(yhook_ipc_type type, const std::string &message)
{
    std::string s;
    yhook_message t;
    t.length = message.size();
    // t.time = millis();
    GetLocalTime(&t.time);
    SYSTEMTIME utc;
    GetSystemTime(&utc);
    t.timestamp = jstime(utc);
    // t.timestamp = gettime(t.time);
    t.type = type;
    s += std::string((const char *)&t, sizeof(yhook_message));
    s += message;
    write_pipe(s);
}

inline void
send_hook_message(const std::string &name,
                  const std::vector<std::pair<std::string, std::string>> &args)
{
    send_message(YHOOK_IPC_HOOK, compose_hook_message(name, args));
}

inline void send_spawn_message()
{
    TCHAR buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    send_message(YHOOK_IPC_SPAWN, compose_spawn_message(tostring(buffer),
                                                        GetCurrentProcessId()));
}

static void yhook_ipc_start()
{
    pipe = CreateFile(TEXT(IPC_PIPE), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, 0, NULL);
}

static void yhook_ipc_close() { CloseHandle(pipe); }

// auto HookedMessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT
// uType)
// {
//     decltype(MessageBox)& OldMessageBox =
//     (decltype(MessageBox)&)hook_map[0].original; write_pipe((uint8_t
//     *)"MessageBox", 11); return OldMessageBox(hWnd, lpText, lpCaption,
//     uType);
// }

// static void DoHookCallback(const char *name)
// {
//     for (size_t i = 0; i < callback_count; i++)
//     {
//         callbacks[i](name);
//     }
// }

LONG YHOOKCALL Hook()
{
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    for (int i = 0; i < yhook_map_length; i++)
    {
        DetourAttach(&yhook_map[i].original, yhook_map[i].detoured);
    }
    // for (int i = 0; i < hook_map_length; i += 2)
    // {
    //     DetourAttach(&hook_map[i], hook_map[i + 1]);
    // }
    return DetourTransactionCommit();
}

LONG YHOOKCALL UnHook()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    for (int i = 0; i < yhook_map_length; i++)
    {
        DetourDetach(&yhook_map[i].original, yhook_map[i].detoured);
    }
    // for (int i = 0; i < hook_map_length; i += 2)
    // {
    //     DetourDetach(&hook_map[i], hook_map[i + 1]);
    // }
    return DetourTransactionCommit();
}

// LONG YHOOKCALL RegisterHookCallback(YHOOKCALLBACK f)
// {
//     callbacks[callback_count++] = f;
// }

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason_for_call, LPVOID lpReserved)
{
    if (DetourIsHelperProcess())
        return TRUE;
    LONG error;
    switch (reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugString(TEXT("Attaching yhook.dll"));
        // send_spawn_message();
        error = Hook();
        if (error == NO_ERROR)
        {
            OutputDebugString(TEXT("Hooking attempt succeeded"));
        }
        else
        {
            OutputDebugString(TEXT("Hooking attempt failed"));
        }
        yhook_ipc_start();
        send_spawn_message();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugString(TEXT("Detaching yhook.dll"));
        error = UnHook();

        if (error == NO_ERROR)
        {
            OutputDebugString(TEXT("Successfully detached hook"));
        }
        else
        {
            OutputDebugString(TEXT("Hook removal has failed"));
        }
        yhook_ipc_close();
        break;
    }
    return TRUE;
}