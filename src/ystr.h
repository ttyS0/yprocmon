#pragma once
#include <sstream>
#include <string>
#include <windows.h>
#include "debug.h"

std::string wstrtostr(const std::wstring &wstr)
{
    // Convert a Unicode string to an ASCII string
    std::string strTo;
    char *szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(),
                        NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

std::wstring strtowstr(const std::string &str)
{
    // Convert an ASCII string to a Unicode String
    std::wstring wstrTo;
    wchar_t *wszTo = new wchar_t[str.length() + 1];
    wszTo[str.size()] = L'\0';
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
    wstrTo = wszTo;
    delete[] wszTo;
    return wstrTo;
}

template <typename T> inline std::string tostring(T v)
{
    return std::to_string(v);
}

inline std::string tostring(LPCSTR v) { return v; }
inline std::string tostring(LPSTR v) { return v; }
inline std::string tostring(LPCWSTR v) { return wstrtostr(v); }
inline std::string tostring(LPWSTR v) { return wstrtostr(v); }
template <typename T> inline std::string tostring(T *v)
{
    std::stringstream ss;
    ss << (void *)v;
    return ss.str();
}

inline std::wstring towstring(const std::string& v) { return strtowstr(v); }
inline std::wstring towstring(const std::wstring& v) { return v; }
// inline std::string tostring(HWND v)
// {
//     std::stringstream ss;
//     ss << v;
//     return ss.str();
// }
// inline std::string tostring(HKEY v)
// {
//     std::stringstream ss;
//     ss << v;
//     return ss.str();
// }
// inline std::string tostring(HANDLE v)
// {
//     std::stringstream ss;
//     ss << v;
//     return ss.str();
// }
// inline std::string tostring(const void* v)
// {
//     std::stringstream ss;
//     ss << v;
//     return ss.str();
// }
// inline std::string tostring(_SECURITY_ATTRIBUTES* v)
// {
//     std::stringstream ss;
//     ss << v;
//     return ss.str();
// }