/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yhookmap.h
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-11
 *
 * @brief Hook 函数以及函数对应表
 */

#include "yhook.h"
#include "ystr.h"
#include <sstream>
#include <string>

int hooked_MessageBox(HWND, LPCWSTR, LPCWSTR, UINT);
HANDLE hooked_CreateFile(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD,
                         DWORD, HANDLE);
WINBOOL hooked_WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
WINBOOL hooked_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
HANDLE hooked_HeapCreate(DWORD, SIZE_T, SIZE_T);
WINBOOL hooked_HeapDestroy(HANDLE);
WINBOOL hooked_HeapFree(HANDLE, DWORD, LPVOID);
LONG hooked_RegCreateKeyEx(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM,
                           LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
LONG hooked_RegSetValueEx(HKEY, LPCWSTR, DWORD, DWORD, const BYTE *, DWORD);
LONG hooked_RegDeleteValue(HKEY, LPCWSTR);
LONG hooked_RegCloseKey(HKEY);
LONG hooked_RegOpenKeyEx(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
SOCKET hooked_socket(int, int, int);
int hooked_bind(SOCKET, const sockaddr *, int);
int hooked_send(SOCKET, const char *, int, int);
int hooked_connect(SOCKET, const sockaddr *, int);
int hooked_recv(SOCKET, char *, int, int);
void *hooked_memcpy(void *__restrict__, const void *__restrict__, size_t);

yhook_entry yhook_map[] = {
    {(PVOID)&MessageBox, (PVOID)&hooked_MessageBox},
    {(PVOID)&CreateFile, (PVOID)&hooked_CreateFile},
    {(PVOID)&WriteFile, (PVOID)&hooked_WriteFile},
    {(PVOID)&ReadFile, (PVOID)&hooked_ReadFile},
    {(PVOID)&HeapCreate, (PVOID)&hooked_HeapCreate},
    {(PVOID)&HeapDestroy, (PVOID)&hooked_HeapDestroy},
    {(PVOID)&HeapFree, (PVOID)&hooked_HeapFree},
    {(PVOID)&RegCreateKeyEx, (PVOID)&hooked_RegCreateKeyEx},
    {(PVOID)&RegSetValueEx, (PVOID)&hooked_RegSetValueEx},
    {(PVOID)&RegDeleteValue, (PVOID)&hooked_RegDeleteValue},
    {(PVOID)&RegCloseKey, (PVOID)&hooked_RegCloseKey},
    {(PVOID)&RegOpenKeyEx, (PVOID)&hooked_RegOpenKeyEx},
    {(PVOID)&socket, (PVOID)&hooked_socket},
    {(PVOID)&bind, (PVOID)&hooked_bind},
    {(PVOID)&send, (PVOID)&hooked_send},
    {(PVOID)&connect, (PVOID)&hooked_connect},
    {(PVOID)&recv, (PVOID)&hooked_recv},
    {(PVOID)&memcpy, (PVOID)&hooked_memcpy},
};
constexpr size_t yhook_map_length = sizeof(yhook_map) / sizeof(yhook_map[0]);

int hooked_MessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{

    send_hook_message("MessageBox", {{"hWnd", tostring(hWnd)},
                                     {"lpText", tostring(lpText)},
                                     {"lpCaption", tostring(lpCaption)},
                                     {"uType", tostring(uType)}});
    decltype(MessageBox) *old_MessageBox =
        (decltype(MessageBox) *)yhook_map[0].original;
    int ret = old_MessageBox(hWnd, lpText, lpCaption, uType);

    return ret;
}
HANDLE hooked_CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess,
                         DWORD dwShareMode,
                         LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         DWORD dwCreationDisposition,
                         DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "CreateFile",
            {{"lpFileName", tostring(lpFileName)},
             {"dwDesiredAccess", tostring(dwDesiredAccess)},
             {"dwShareMode", tostring(dwShareMode)},
             {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
             {"dwCreationDisposition", tostring(dwCreationDisposition)},
             {"dwFlagsAndAttributes", tostring(dwFlagsAndAttributes)},
             {"hTemplateFile", tostring(hTemplateFile)}});
        entered = false;
    }
    decltype(CreateFile) *old_CreateFile =
        (decltype(CreateFile) *)yhook_map[1].original;
    HANDLE ret = old_CreateFile(lpFileName, dwDesiredAccess, dwShareMode,
                                lpSecurityAttributes, dwCreationDisposition,
                                dwFlagsAndAttributes, hTemplateFile);

    return ret;
}
WINBOOL hooked_WriteFile(HANDLE hFile, LPCVOID lpBuffer,
                         DWORD nNumberOfBytesToWrite,
                         LPDWORD lpNumberOfBytesWritten,
                         LPOVERLAPPED lpOverlapped)
{
    static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "WriteFile",
            {{"hFile", tostring(hFile)},
             {"lpBuffer", tostring(lpBuffer)},
             {"nNumberOfBytesToWrite", tostring(nNumberOfBytesToWrite)},
             {"lpNumberOfBytesWritten", tostring(lpNumberOfBytesWritten)},
             {"lpOverlapped", tostring(lpOverlapped)}});
        entered = false;
    }
    decltype(WriteFile) *old_WriteFile =
        (decltype(WriteFile) *)yhook_map[2].original;
    WINBOOL ret = old_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
                                lpNumberOfBytesWritten, lpOverlapped);

    return ret;
}
WINBOOL hooked_ReadFile(HANDLE hFile, LPVOID lpBuffer,
                        DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                        LPOVERLAPPED lpOverlapped)
{

    send_hook_message("ReadFile",
                      {{"hFile", tostring(hFile)},
                       {"lpBuffer", tostring(lpBuffer)},
                       {"nNumberOfBytesToRead", tostring(nNumberOfBytesToRead)},
                       {"lpNumberOfBytesRead", tostring(lpNumberOfBytesRead)},
                       {"lpOverlapped", tostring(lpOverlapped)}});
    decltype(ReadFile) *old_ReadFile =
        (decltype(ReadFile) *)yhook_map[3].original;
    WINBOOL ret = old_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
                               lpNumberOfBytesRead, lpOverlapped);

    return ret;
}
HANDLE hooked_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize,
                         SIZE_T dwMaximumSize)
{

    send_hook_message("HeapCreate",
                      {{"flOptions", tostring(flOptions)},
                       {"dwInitialSize", tostring(dwInitialSize)},
                       {"dwMaximumSize", tostring(dwMaximumSize)}});
    decltype(HeapCreate) *old_HeapCreate =
        (decltype(HeapCreate) *)yhook_map[4].original;
    HANDLE ret = old_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);

    return ret;
}
WINBOOL hooked_HeapDestroy(HANDLE hHeap)
{

    send_hook_message("HeapDestroy", {{"hHeap", tostring(hHeap)}});
    decltype(HeapDestroy) *old_HeapDestroy =
        (decltype(HeapDestroy) *)yhook_map[5].original;
    WINBOOL ret = old_HeapDestroy(hHeap);

    return ret;
}
WINBOOL hooked_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{

    send_hook_message("HeapFree", {{"hHeap", tostring(hHeap)},
                                   {"dwFlags", tostring(dwFlags)},
                                   {"lpMem", tostring(lpMem)}});
    decltype(HeapFree) *old_HeapFree =
        (decltype(HeapFree) *)yhook_map[6].original;
    WINBOOL ret = old_HeapFree(hHeap, dwFlags, lpMem);

    return ret;
}
LONG hooked_RegCreateKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved,
                           LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                           LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                           PHKEY phkResult, LPDWORD lpdwDisposition)
{

    send_hook_message("RegCreateKeyEx",
                      {{"hKey", tostring(hKey)},
                       {"lpSubKey", tostring(lpSubKey)},
                       {"Reserved", tostring(Reserved)},
                       {"lpClass", tostring(lpClass)},
                       {"dwOptions", tostring(dwOptions)},
                       {"samDesired", tostring(samDesired)},
                       {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
                       {"phkResult", tostring(phkResult)},
                       {"lpdwDisposition", tostring(lpdwDisposition)}});
    decltype(RegCreateKeyEx) *old_RegCreateKeyEx =
        (decltype(RegCreateKeyEx) *)yhook_map[7].original;
    LONG ret = old_RegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions,
                                  samDesired, lpSecurityAttributes, phkResult,
                                  lpdwDisposition);

    return ret;
}
LONG hooked_RegSetValueEx(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved,
                          DWORD dwType, const BYTE *lpData, DWORD cbData)
{

    send_hook_message("RegSetValueEx", {{"hKey", tostring(hKey)},
                                        {"lpValueName", tostring(lpValueName)},
                                        {"Reserved", tostring(Reserved)},
                                        {"dwType", tostring(dwType)},
                                        {"lpData", tostring(lpData)},
                                        {"cbData", tostring(cbData)}});
    decltype(RegSetValueEx) *old_RegSetValueEx =
        (decltype(RegSetValueEx) *)yhook_map[8].original;
    LONG ret =
        old_RegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);

    return ret;
}
LONG hooked_RegDeleteValue(HKEY hKey, LPCWSTR lpValueName)
{

    send_hook_message(
        "RegDeleteValue",
        {{"hKey", tostring(hKey)}, {"lpValueName", tostring(lpValueName)}});
    decltype(RegDeleteValue) *old_RegDeleteValue =
        (decltype(RegDeleteValue) *)yhook_map[9].original;
    LONG ret = old_RegDeleteValue(hKey, lpValueName);

    return ret;
}
LONG hooked_RegCloseKey(HKEY hKey)
{

    send_hook_message("RegCloseKey", {{"hKey", tostring(hKey)}});
    decltype(RegCloseKey) *old_RegCloseKey =
        (decltype(RegCloseKey) *)yhook_map[10].original;
    LONG ret = old_RegCloseKey(hKey);

    return ret;
}
LONG hooked_RegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                         REGSAM samDesired, PHKEY phkResult)
{

    send_hook_message("RegOpenKeyEx", {{"hKey", tostring(hKey)},
                                       {"lpSubKey", tostring(lpSubKey)},
                                       {"ulOptions", tostring(ulOptions)},
                                       {"samDesired", tostring(samDesired)},
                                       {"phkResult", tostring(phkResult)}});
    decltype(RegOpenKeyEx) *old_RegOpenKeyEx =
        (decltype(RegOpenKeyEx) *)yhook_map[11].original;
    LONG ret =
        old_RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);

    return ret;
}
SOCKET hooked_socket(int af, int type, int protocol)
{

    send_hook_message("socket", {{"af", tostring(af)},
                                 {"type", tostring(type)},
                                 {"protocol", tostring(protocol)}});
    decltype(socket) *old_socket = (decltype(socket) *)yhook_map[12].original;
    SOCKET ret = old_socket(af, type, protocol);

    return ret;
}
int hooked_bind(SOCKET s, const sockaddr *name, int namelen)
{

    send_hook_message("bind", {{"s", tostring(s)},
                               {"name", tostring(name)},
                               {"namelen", tostring(namelen)}});
    decltype(bind) *old_bind = (decltype(bind) *)yhook_map[13].original;
    int ret = old_bind(s, name, namelen);

    return ret;
}
int hooked_send(SOCKET s, const char *buf, int len, int flags)
{

    send_hook_message("send", {{"s", tostring(s)},
                               {"buf", tostring(buf)},
                               {"len", tostring(len)},
                               {"flags", tostring(flags)},
                               {"data", std::string(buf, len)}});
    decltype(send) *old_send = (decltype(send) *)yhook_map[14].original;
    int ret = old_send(s, buf, len, flags);

    return ret;
}
int hooked_connect(SOCKET s, const sockaddr *name, int namelen)
{

    send_hook_message("connect", {{"s", tostring(s)},
                                  {"name", tostring(name)},
                                  {"namelen", tostring(namelen)}});
    decltype(connect) *old_connect =
        (decltype(connect) *)yhook_map[15].original;
    int ret = old_connect(s, name, namelen);

    return ret;
}
int hooked_recv(SOCKET s, char *buf, int len, int flags)
{

    send_hook_message("recv", {{"s", tostring(s)},
                               {"buf", tostring(buf)},
                               {"len", tostring(len)},
                               {"flags", tostring(flags)},
                               {"data", std::string(buf, len)}});
    decltype(recv) *old_recv = (decltype(recv) *)yhook_map[16].original;
    int ret = old_recv(s, buf, len, flags);

    return ret;
}
void *hooked_memcpy(void *__restrict__ __dst, const void *__restrict__ __src,
                    size_t __n)
{
    static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("memcpy", {{"src", std::string((char *)__src, __n)},
                                     {"dst", std::string((char *)__dst, __n)}});
        entered = false;
    }
    decltype(memcpy) *old_memcpy = (decltype(memcpy) *)yhook_map[17].original;
    void *ret = old_memcpy(__dst, __src, __n);

    return ret;
}
