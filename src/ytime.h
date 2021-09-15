#include <time.h>
#include <windows.h>
#include <cstdint>

time_t unixtime(const SYSTEMTIME &t)
{
    tm tm;
    tm.tm_sec = t.wSecond;
    tm.tm_min = t.wMinute;
    tm.tm_hour = t.wHour;
    tm.tm_mday = t.wDay;
    tm.tm_mon = t.wMonth - 1;
    tm.tm_year = t.wYear - 1900;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

uint64_t jstime(const SYSTEMTIME &t)
{
    uint64_t r = unixtime(t);
    r *= (uint64_t) 1000;
    r += (uint64_t) t.wMilliseconds;
    return r;
}