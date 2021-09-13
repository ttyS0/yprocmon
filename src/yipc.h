/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yipc.h
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-12
 *
 * @brief IPC 协议相关结构和函数
 */

#pragma once
#define IPC_PIPE "\\\\.\\pipe\\YhookPipe"

enum yhook_ipc_type
{
    YHOOK_IPC_RESERVED,
    YHOOK_IPC_SPAWN,
    YHOOK_IPC_HOOK,
    YHOOK_IPC_NET,
    YHOOK_IPC_MEMFLOW
};

const char *yhook_ipc_type_text[] = {"RESERVED", "SPAWN", "HOOK"};

struct yhook_ipc_hook_arg
{
    char arg_name[0];
};

struct yhook_ipc_spawn
{
    std::string process;
    DWORD pid;
    std::vector<std::pair<std::string, std::string>> args;
};

struct yhook_ipc_hook
{
    std::string name;
    std::vector<std::pair<std::string, std::string>> args;
};

struct yhook_message
{
    time_t timestamp;
    SYSTEMTIME time;
    yhook_ipc_type type;
    size_t length;
    char message[0];
};

// a unified entry to be stored in state
struct yhook_message_entry
{
    time_t timestamp;
    SYSTEMTIME time;
    yhook_ipc_type type;
    // union
    // {
    yhook_ipc_spawn spawn;
    yhook_ipc_hook hook;
    // };
    // ~yhook_message_entry() {};
    template <typename Writer> void Serialize(Writer &writer) const
    {
        char asctime[15];
        snprintf(asctime, 15, "%02d:%02d:%02d.%03d", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
        writer.StartObject();
        writer.String("timestamp");
        writer.Uint(timestamp);
        writer.String("time");
        writer.String(asctime);
        writer.String("type");
        writer.String(yhook_ipc_type_text[type]);
        writer.String("data");
        if (type == YHOOK_IPC_SPAWN)
        {
            writer.StartObject();
            writer.String("process");
            writer.String(spawn.process);
            writer.String("pid");
            writer.Uint(spawn.pid);
            writer.EndObject();
        }
        else if (type == YHOOK_IPC_HOOK)
        {
            writer.StartObject();
            writer.String("name");
            writer.String(hook.name);
            writer.String("args");
            writer.StartArray();
            for (auto const &arg : hook.args)
            {
                writer.StartObject();
                writer.String("name");
                writer.String(arg.first);
                writer.String("value");
                writer.String(arg.second);
                writer.EndObject();
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndObject();
    }

    // bool operator<(const yhook_message_entry& l, const yhook_message_entry& r)
    // {
    //     return l.timestamp < r.timestamp;
    // }
};

bool operator<(const yhook_message_entry& l, const yhook_message_entry& r)
{
    return l.timestamp < r.timestamp;
}
bool operator<(const yhook_message_entry& l, const time_t& r)
{
    return l.timestamp < r;
}
bool operator<(const time_t& l, const yhook_message_entry& r)
{
    return l < r.timestamp;
}

// Hook message
// str[NUL] name
// size_t   arg_count
// [
//    str[NUL]      arg_name
//    size_t        length
//    str[length]   arg_value
// ] * arg_count
const std::string compose_hook_message(
    const std::string &name,
    const std::vector<std::pair<std::string, std::string>> &args)
{
    std::string s;
    s += name;
    s += '\0';
    size_t arg_count = args.size();
    s += std::string((const char *)&arg_count, sizeof(size_t));
    for (auto const &arg : args)
    {
        s += arg.first;
        s += '\0';
        size_t length = arg.second.size();
        s += std::string((const char *)&length, sizeof(size_t));
        s += arg.second;
    }
    return s;
}
yhook_ipc_hook parse_hook_message(const std::string &message)
{
    yhook_ipc_hook t;
    const char *p = message.c_str();
    t.name = p;
    p += t.name.size();
    p += 1; // NUL
    size_t arg_count = *(size_t *)p;
    p += sizeof(arg_count);
    for (size_t i = 0; i < arg_count; i++)
    {
        std::pair<std::string, std::string> arg;
        arg.first = std::string(p);
        p += arg.first.size();
        p += 1; // NUL
        size_t length = *(size_t *)p;
        p += sizeof(length);
        arg.second = std::string(p, length);
        p += length;
        t.args.push_back(arg);
    }
    return t;
}
const std::string compose_spawn_message(std::string process, DWORD pid)
{
    std::string s;
    s += process;
    s += '\0';
    s += std::string((const char *)&pid, sizeof(pid));
    return s;
}
yhook_ipc_spawn parse_spawn_message(const std::string &message)
{
    yhook_ipc_spawn t;
    const char *p = message.c_str();
    t.process = p;
    p += t.process.size();
    p += 1;
    DWORD pid = *(DWORD *)p;
    t.pid = pid;
    p += sizeof(pid);
    return t;
}