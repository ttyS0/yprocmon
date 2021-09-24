#include "yipc.h"
#include <cstdint>
#include <string>
#include <vector>
#include <windows.h>

HANDLE pipe;

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
    t.timestamp = 0;
    // t.timestamp = gettime(t.time);
    t.type = type;
    s += std::string((const char *)&t, sizeof(yhook_message));
    s += message;
    write_pipe(s);
}

int main()
{
    pipe = CreateFile(TEXT(IPC_PIPE), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, 0, NULL);
    while (true)
    {
        send_message(yhook_ipc_type::YHOOK_IPC_HOOK, compose_hook_message("A", {}));
        Sleep(5000);
    }
}