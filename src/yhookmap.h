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
    return old_MessageBox(hWnd, lpText, lpCaption, uType);
}
HANDLE hooked_CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess,
                         DWORD dwShareMode,
                         LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         DWORD dwCreationDisposition,
                         DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    send_hook_message(
        "CreateFile",
        {{"lpFileName", tostring(lpFileName)},
         {"dwDesiredAccess", tostring(dwDesiredAccess)},
         {"dwShareMode", tostring(dwShareMode)},
         {"lpSecurityAttributes", tostring(lpSecurityAttributes)},
         {"dwCreationDisposition", tostring(dwCreationDisposition)},
         {"dwFlagsAndAttributes", tostring(dwFlagsAndAttributes)},
         {"hTemplateFile", tostring(hTemplateFile)}});
    decltype(CreateFile) *old_CreateFile =
        (decltype(CreateFile) *)yhook_map[1].original;
    return old_CreateFile(lpFileName, dwDesiredAccess, dwShareMode,
                          lpSecurityAttributes, dwCreationDisposition,
                          dwFlagsAndAttributes, hTemplateFile);
}
WINBOOL hooked_WriteFile(HANDLE hFile, LPCVOID lpBuffer,
                         DWORD nNumberOfBytesToWrite,
                         LPDWORD lpNumberOfBytesWritten,
                         LPOVERLAPPED lpOverlapped)
{
    if (hFile != pipe)
    {
        send_hook_message(
            "WriteFile",
            {{"hFile", tostring(hFile)},
             {"lpBuffer", tostring(lpBuffer)},
             {"nNumberOfBytesToWrite", tostring(nNumberOfBytesToWrite)},
             {"lpNumberOfBytesWritten", tostring(lpNumberOfBytesWritten)},
             {"lpOverlapped", tostring(lpOverlapped)}});
    }
    decltype(WriteFile) *old_WriteFile =
        (decltype(WriteFile) *)yhook_map[2].original;
    return old_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
                         lpNumberOfBytesWritten, lpOverlapped);
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
    return old_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
                        lpNumberOfBytesRead, lpOverlapped);
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
    return old_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
}
WINBOOL hooked_HeapDestroy(HANDLE hHeap)
{
    send_hook_message("HeapDestroy", {{"hHeap", tostring(hHeap)}});
    decltype(HeapDestroy) *old_HeapDestroy =
        (decltype(HeapDestroy) *)yhook_map[5].original;
    return old_HeapDestroy(hHeap);
}
WINBOOL hooked_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
    send_hook_message("HeapFree", {{"hHeap", tostring(hHeap)},
                                   {"dwFlags", tostring(dwFlags)},
                                   {"lpMem", tostring(lpMem)}});
    decltype(HeapFree) *old_HeapFree =
        (decltype(HeapFree) *)yhook_map[6].original;
    return old_HeapFree(hHeap, dwFlags, lpMem);
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
    return old_RegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions,
                              samDesired, lpSecurityAttributes, phkResult,
                              lpdwDisposition);
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
    return old_RegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData,
                             cbData);
}
LONG hooked_RegDeleteValue(HKEY hKey, LPCWSTR lpValueName)
{
    send_hook_message(
        "RegDeleteValue",
        {{"hKey", tostring(hKey)}, {"lpValueName", tostring(lpValueName)}});
    decltype(RegDeleteValue) *old_RegDeleteValue =
        (decltype(RegDeleteValue) *)yhook_map[9].original;
    return old_RegDeleteValue(hKey, lpValueName);
}
LONG hooked_RegCloseKey(HKEY hKey)
{
    send_hook_message("RegCloseKey", {{"hKey", tostring(hKey)}});
    decltype(RegCloseKey) *old_RegCloseKey =
        (decltype(RegCloseKey) *)yhook_map[10].original;
    return old_RegCloseKey(hKey);
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
    return old_RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}
SOCKET hooked_socket(int af, int type, int protocol)
{
    send_hook_message("socket", {{"af", tostring(af)},
                                 {"type", tostring(type)},
                                 {"protocol", tostring(protocol)}});
    decltype(socket) *old_socket = (decltype(socket) *)yhook_map[12].original;
    return old_socket(af, type, protocol);
}
int hooked_bind(SOCKET s, const sockaddr *name, int namelen)
{
    send_hook_message("bind", {{"s", tostring(s)},
                               {"name", tostring(name)},
                               {"namelen", tostring(namelen)}});
    decltype(bind) *old_bind = (decltype(bind) *)yhook_map[13].original;
    return old_bind(s, name, namelen);
}
int hooked_send(SOCKET s, const char *buf, int len, int flags)
{
    send_hook_message("send", {{"s", tostring(s)},
                               {"buf", tostring(buf)},
                               {"len", tostring(len)},
                               {"flags", tostring(flags)}});
    decltype(send) *old_send = (decltype(send) *)yhook_map[14].original;
    return old_send(s, buf, len, flags);
}
int hooked_connect(SOCKET s, const sockaddr *name, int namelen)
{
    send_hook_message("connect", {{"s", tostring(s)},
                                  {"name", tostring(name)},
                                  {"namelen", tostring(namelen)}});
    decltype(connect) *old_connect =
        (decltype(connect) *)yhook_map[15].original;
    return old_connect(s, name, namelen);
}
int hooked_recv(SOCKET s, char *buf, int len, int flags)
{
    send_hook_message("recv", {{"s", tostring(s)},
                               {"buf", tostring(buf)},
                               {"len", tostring(len)},
                               {"flags", tostring(flags)}});
    decltype(recv) *old_recv = (decltype(recv) *)yhook_map[16].original;
    return old_recv(s, buf, len, flags);
}
