#include "yhook.h"
#include "ystr.h"
#include <sstream>
#include <string>

bool entered = false;

int hooked_MessageBoxA(HWND, LPCSTR, LPCSTR, UINT);
int hooked_MessageBoxW(HWND, LPCWSTR, LPCWSTR, UINT);
HANDLE hooked_CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD,
                          DWORD, HANDLE);
HANDLE hooked_CreateFileW(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD,
                          DWORD, HANDLE);
WINBOOL hooked_WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
WINBOOL hooked_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
HANDLE hooked_HeapCreate(DWORD, SIZE_T, SIZE_T);
WINBOOL hooked_HeapDestroy(HANDLE);
WINBOOL hooked_HeapFree(HANDLE, DWORD, LPVOID);
LONG hooked_RegCreateKeyExA(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM,
                            LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
LONG hooked_RegCreateKeyExW(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM,
                            LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
LONG hooked_RegSetValueExA(HKEY, LPCSTR, DWORD, DWORD, const BYTE *, DWORD);
LONG hooked_RegSetValueExW(HKEY, LPCWSTR, DWORD, DWORD, const BYTE *, DWORD);
LONG hooked_RegDeleteValueA(HKEY, LPCSTR);
LONG hooked_RegDeleteValueW(HKEY, LPCWSTR);
LONG hooked_RegCloseKey(HKEY);
LONG hooked_RegOpenKeyEx(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
SOCKET hooked_socket(int, int, int);
int hooked_bind(SOCKET, const sockaddr *, int);
int hooked_send(SOCKET, const char *, int, int);
int hooked_connect(SOCKET, const sockaddr *, int);
int hooked_recv(SOCKET, char *, int, int);
void *hooked_memcpy(void *__restrict__, const void *__restrict__, size_t);

yhook_entry yhook_map[] = {
    {(PVOID)&MessageBoxA, (PVOID)&hooked_MessageBoxA},
    {(PVOID)&MessageBoxW, (PVOID)&hooked_MessageBoxW},
    {(PVOID)&CreateFileA, (PVOID)&hooked_CreateFileA},
    {(PVOID)&CreateFileW, (PVOID)&hooked_CreateFileW},
    {(PVOID)&WriteFile, (PVOID)&hooked_WriteFile},
    {(PVOID)&ReadFile, (PVOID)&hooked_ReadFile},
    {(PVOID)&HeapCreate, (PVOID)&hooked_HeapCreate},
    {(PVOID)&HeapDestroy, (PVOID)&hooked_HeapDestroy},
    {(PVOID)&HeapFree, (PVOID)&hooked_HeapFree},
    {(PVOID)&RegCreateKeyExA, (PVOID)&hooked_RegCreateKeyExA},
    {(PVOID)&RegCreateKeyExW, (PVOID)&hooked_RegCreateKeyExW},
    {(PVOID)&RegSetValueExA, (PVOID)&hooked_RegSetValueExA},
    {(PVOID)&RegSetValueExW, (PVOID)&hooked_RegSetValueExW},
    {(PVOID)&RegDeleteValueA, (PVOID)&hooked_RegDeleteValueA},
    {(PVOID)&RegDeleteValueW, (PVOID)&hooked_RegDeleteValueW},
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

int hooked_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("MessageBoxA", {{"hWnd", tostring(hWnd)},
                                          {"lpText", tostring(lpText)},
                                          {"lpCaption", tostring(lpCaption)},
                                          {"uType", tostring(uType)}});
        entered = false;
    }
    decltype(MessageBoxA) *old_MessageBoxA =
        (decltype(MessageBoxA) *)yhook_map[0].original;
    int ret = old_MessageBoxA(hWnd, lpText, lpCaption, uType);

    return ret;
}
int hooked_MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("MessageBoxW", {{"hWnd", tostring(hWnd)},
                                          {"lpText", tostring(lpText)},
                                          {"lpCaption", tostring(lpCaption)},
                                          {"uType", tostring(uType)}});
        entered = false;
    }
    decltype(MessageBoxW) *old_MessageBoxW =
        (decltype(MessageBoxW) *)yhook_map[1].original;
    int ret = old_MessageBoxW(hWnd, lpText, lpCaption, uType);

    return ret;
}
HANDLE hooked_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess,
                          DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                          DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "CreateFileA",
            {{"lpFileName", tostring(lpFileName)},
             {"dwDesiredAccess", tostring(dwDesiredAccess)},
             {"dwShareMode", tostring(dwShareMode)},
             {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
             {"dwCreationDisposition", tostring(dwCreationDisposition)},
             {"dwFlagsAndAttributes", tostring(dwFlagsAndAttributes)},
             {"hTemplateFile", tostring(hTemplateFile)}});
        entered = false;
    }
    decltype(CreateFileA) *old_CreateFileA =
        (decltype(CreateFileA) *)yhook_map[2].original;
    HANDLE ret = old_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode,
                                 lpSecurityAttributes, dwCreationDisposition,
                                 dwFlagsAndAttributes, hTemplateFile);

    return ret;
}
HANDLE hooked_CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess,
                          DWORD dwShareMode,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                          DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "CreateFileW",
            {{"lpFileName", tostring(lpFileName)},
             {"dwDesiredAccess", tostring(dwDesiredAccess)},
             {"dwShareMode", tostring(dwShareMode)},
             {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
             {"dwCreationDisposition", tostring(dwCreationDisposition)},
             {"dwFlagsAndAttributes", tostring(dwFlagsAndAttributes)},
             {"hTemplateFile", tostring(hTemplateFile)}});
        entered = false;
    }
    decltype(CreateFileW) *old_CreateFileW =
        (decltype(CreateFileW) *)yhook_map[3].original;
    HANDLE ret = old_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                                 lpSecurityAttributes, dwCreationDisposition,
                                 dwFlagsAndAttributes, hTemplateFile);

    return ret;
}
WINBOOL hooked_WriteFile(HANDLE hFile, LPCVOID lpBuffer,
                         DWORD nNumberOfBytesToWrite,
                         LPDWORD lpNumberOfBytesWritten,
                         LPOVERLAPPED lpOverlapped)
{
    // static bool entered = false;
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
        (decltype(WriteFile) *)yhook_map[4].original;
    WINBOOL ret = old_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
                                lpNumberOfBytesWritten, lpOverlapped);

    return ret;
}
WINBOOL hooked_ReadFile(HANDLE hFile, LPVOID lpBuffer,
                        DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                        LPOVERLAPPED lpOverlapped)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "ReadFile",
            {{"hFile", tostring(hFile)},
             {"lpBuffer", tostring(lpBuffer)},
             {"nNumberOfBytesToRead", tostring(nNumberOfBytesToRead)},
             {"lpNumberOfBytesRead", tostring(lpNumberOfBytesRead)},
             {"lpOverlapped", tostring(lpOverlapped)}});
        entered = false;
    }
    decltype(ReadFile) *old_ReadFile =
        (decltype(ReadFile) *)yhook_map[5].original;
    WINBOOL ret = old_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
                               lpNumberOfBytesRead, lpOverlapped);

    return ret;
}
HANDLE hooked_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize,
                         SIZE_T dwMaximumSize)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("HeapCreate",
                          {{"flOptions", tostring(flOptions)},
                           {"dwInitialSize", tostring(dwInitialSize)},
                           {"dwMaximumSize", tostring(dwMaximumSize)}});
        entered = false;
    }
    decltype(HeapCreate) *old_HeapCreate =
        (decltype(HeapCreate) *)yhook_map[6].original;
    HANDLE ret = old_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);

    return ret;
}
WINBOOL hooked_HeapDestroy(HANDLE hHeap)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("HeapDestroy", {{"hHeap", tostring(hHeap)}});
        entered = false;
    }
    decltype(HeapDestroy) *old_HeapDestroy =
        (decltype(HeapDestroy) *)yhook_map[7].original;
    WINBOOL ret = old_HeapDestroy(hHeap);

    return ret;
}
WINBOOL hooked_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("HeapFree", {{"hHeap", tostring(hHeap)},
                                       {"dwFlags", tostring(dwFlags)},
                                       {"lpMem", tostring(lpMem)}});
        entered = false;
    }
    decltype(HeapFree) *old_HeapFree =
        (decltype(HeapFree) *)yhook_map[8].original;
    WINBOOL ret = old_HeapFree(hHeap, dwFlags, lpMem);

    return ret;
}
LONG hooked_RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved,
                            LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                            PHKEY phkResult, LPDWORD lpdwDisposition)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "RegCreateKeyExA",
            {{"hKey", tostring(hKey)},
             {"lpSubKey", tostring(lpSubKey)},
             {"Reserved", tostring(Reserved)},
             {"lpClass", tostring(lpClass)},
             {"dwOptions", tostring(dwOptions)},
             {"samDesired", tostring(samDesired)},
             {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
             {"phkResult", tostring(phkResult)},
             {"lpdwDisposition", tostring(lpdwDisposition)}});
        entered = false;
    }
    decltype(RegCreateKeyExA) *old_RegCreateKeyExA =
        (decltype(RegCreateKeyExA) *)yhook_map[9].original;
    LONG ret = old_RegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions,
                                   samDesired, lpSecurityAttributes, phkResult,
                                   lpdwDisposition);

    return ret;
}
LONG hooked_RegCreateKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved,
                            LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                            PHKEY phkResult, LPDWORD lpdwDisposition)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "RegCreateKeyExW",
            {{"hKey", tostring(hKey)},
             {"lpSubKey", tostring(lpSubKey)},
             {"Reserved", tostring(Reserved)},
             {"lpClass", tostring(lpClass)},
             {"dwOptions", tostring(dwOptions)},
             {"samDesired", tostring(samDesired)},
             {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
             {"phkResult", tostring(phkResult)},
             {"lpdwDisposition", tostring(lpdwDisposition)}});
        entered = false;
    }
    decltype(RegCreateKeyExW) *old_RegCreateKeyExW =
        (decltype(RegCreateKeyExW) *)yhook_map[10].original;
    LONG ret = old_RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions,
                                   samDesired, lpSecurityAttributes, phkResult,
                                   lpdwDisposition);

    return ret;
}
LONG hooked_RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved,
                           DWORD dwType, const BYTE *lpData, DWORD cbData)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("RegSetValueExA",
                          {{"hKey", tostring(hKey)},
                           {"lpValueName", tostring(lpValueName)},
                           {"Reserved", tostring(Reserved)},
                           {"dwType", tostring(dwType)},
                           {"lpData", tostring(lpData)},
                           {"cbData", tostring(cbData)}});
        entered = false;
    }
    decltype(RegSetValueExA) *old_RegSetValueExA =
        (decltype(RegSetValueExA) *)yhook_map[11].original;
    LONG ret =
        old_RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);

    return ret;
}
LONG hooked_RegSetValueExW(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved,
                           DWORD dwType, const BYTE *lpData, DWORD cbData)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("RegSetValueExW",
                          {{"hKey", tostring(hKey)},
                           {"lpValueName", tostring(lpValueName)},
                           {"Reserved", tostring(Reserved)},
                           {"dwType", tostring(dwType)},
                           {"lpData", tostring(lpData)},
                           {"cbData", tostring(cbData)}});
        entered = false;
    }
    decltype(RegSetValueExW) *old_RegSetValueExW =
        (decltype(RegSetValueExW) *)yhook_map[12].original;
    LONG ret =
        old_RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);

    return ret;
}
LONG hooked_RegDeleteValueA(HKEY hKey, LPCSTR lpValueName)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "RegDeleteValueA",
            {{"hKey", tostring(hKey)}, {"lpValueName", tostring(lpValueName)}});
        entered = false;
    }
    decltype(RegDeleteValueA) *old_RegDeleteValueA =
        (decltype(RegDeleteValueA) *)yhook_map[13].original;
    LONG ret = old_RegDeleteValueA(hKey, lpValueName);

    return ret;
}
LONG hooked_RegDeleteValueW(HKEY hKey, LPCWSTR lpValueName)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "RegDeleteValueW",
            {{"hKey", tostring(hKey)}, {"lpValueName", tostring(lpValueName)}});
        entered = false;
    }
    decltype(RegDeleteValueW) *old_RegDeleteValueW =
        (decltype(RegDeleteValueW) *)yhook_map[14].original;
    LONG ret = old_RegDeleteValueW(hKey, lpValueName);

    return ret;
}
LONG hooked_RegCloseKey(HKEY hKey)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("RegCloseKey", {{"hKey", tostring(hKey)}});
        entered = false;
    }
    decltype(RegCloseKey) *old_RegCloseKey =
        (decltype(RegCloseKey) *)yhook_map[15].original;
    LONG ret = old_RegCloseKey(hKey);

    return ret;
}
LONG hooked_RegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                         REGSAM samDesired, PHKEY phkResult)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("RegOpenKeyEx", {{"hKey", tostring(hKey)},
                                           {"lpSubKey", tostring(lpSubKey)},
                                           {"ulOptions", tostring(ulOptions)},
                                           {"samDesired", tostring(samDesired)},
                                           {"phkResult", tostring(phkResult)}});
        entered = false;
    }
    decltype(RegOpenKeyEx) *old_RegOpenKeyEx =
        (decltype(RegOpenKeyEx) *)yhook_map[16].original;
    LONG ret =
        old_RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);

    return ret;
}
SOCKET hooked_socket(int af, int type, int protocol)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("socket", {{"af", tostring(af)},
                                     {"type", tostring(type)},
                                     {"protocol", tostring(protocol)}});
        entered = false;
    }
    decltype(socket) *old_socket = (decltype(socket) *)yhook_map[17].original;
    SOCKET ret = old_socket(af, type, protocol);

    return ret;
}
int hooked_bind(SOCKET s, const sockaddr *name, int namelen)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message(
            "bind",
            {{"s", tostring(s)},
             {"name", tostring(name)},
             {"namelen", tostring(namelen)},
             {"addr", std::string(inet_ntoa(((sockaddr_in *)name)->sin_addr))},
             {"port", tostring(htons(((sockaddr_in *)name)->sin_port))}});
        entered = false;
    }
    decltype(bind) *old_bind = (decltype(bind) *)yhook_map[18].original;
    int ret = old_bind(s, name, namelen);

    return ret;
}
int hooked_send(SOCKET s, const char *buf, int len, int flags)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("send", {{"s", tostring(s)},
                                   {"buf", tostring(buf)},
                                   {"len", tostring(len)},
                                   {"flags", tostring(flags)},
                                   {"data", std::string(buf, len)}});
        entered = false;
    }
    decltype(send) *old_send = (decltype(send) *)yhook_map[19].original;
    int ret = old_send(s, buf, len, flags);

    return ret;
}
int hooked_connect(SOCKET s, const sockaddr *name, int namelen)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("connect", {{"s", tostring(s)},
                                      {"name", tostring(name)},
                                      {"namelen", tostring(namelen)}});
        entered = false;
    }
    decltype(connect) *old_connect =
        (decltype(connect) *)yhook_map[20].original;
    int ret = old_connect(s, name, namelen);

    return ret;
}
int hooked_recv(SOCKET s, char *buf, int len, int flags)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("recv", {{"s", tostring(s)},
                                   {"buf", tostring(buf)},
                                   {"len", tostring(len)},
                                   {"flags", tostring(flags)},
                                   {"data", std::string(buf, len)}});
        entered = false;
    }
    decltype(recv) *old_recv = (decltype(recv) *)yhook_map[21].original;
    int ret = old_recv(s, buf, len, flags);

    return ret;
}
void *hooked_memcpy(void *__restrict__ __dst, const void *__restrict__ __src,
                    size_t __n)
{
    // static bool entered = false;
    if (!entered)
    {
        entered = true;

        send_hook_message("memcpy", {{"__dst", tostring(__dst)},
                                     {"__src", tostring(__src)},
                                     {"__n", tostring(__n)},
                                     {"src", std::string((char *)__src, __n)},
                                     {"dst", std::string((char *)__dst, __n)}});
        entered = false;
    }
    decltype(memcpy) *old_memcpy = (decltype(memcpy) *)yhook_map[22].original;
    void *ret = old_memcpy(__dst, __src, __n);

    return ret;
}
