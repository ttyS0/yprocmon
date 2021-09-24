/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yprocmon.cc
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief yprocmon 头文件
 */

#pragma once

#include "yipc.h"
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <windows.h>

#define QUOTE(x) #x
#define QUOTE_EXPANDED(x) QUOTE(x)
#define YPROCMON_HTTP_PORT 8051
#define YPROCMON_WEBSOCKET_PORT 8052
#define YPROCMON_HTTP_URL "http://localhost:" QUOTE_EXPANDED(YPROCMON_HTTP_PORT)
#define SAMPLE_DIR "samples"

struct file_entry
{
    std::string name;
    uint64_t timestamp;
    size_t size;
    template <typename Writer> void Serialize(Writer &writer) const
    {
        writer.StartObject();
        writer.String("name");
        writer.String(name);
        writer.String("timestamp");
        writer.Uint64(timestamp);
        writer.String("size");
        writer.Uint(size);
        writer.EndObject();
    }
};

enum instance_status
{
    INSTANCE_SPAWNED,
    INSTANCE_EXITED
};

struct instance_entry
{
    std::thread waiting;
    std::string name;
    uint64_t timestamp;
    PROCESS_INFORMATION pi;
    instance_status status;
    DWORD exitCode;
    template <typename Writer> void Serialize(Writer &writer) const
    {
        writer.StartObject();
        writer.String("name");
        writer.String(name);
        writer.String("timestamp");
        writer.Uint64(timestamp);
        writer.String("pid");
        writer.Uint(pi.dwProcessId);
        writer.String("tid");
        writer.Uint(pi.dwThreadId);
        writer.String("status");
        writer.String(status == INSTANCE_SPAWNED ? "spawned" : "exited");
        writer.String("exitCode");
        writer.Int(exitCode);
        writer.EndObject();
    }
};

struct message_entry
{
    uint64_t timestamp;
    SYSTEMTIME time;
    DWORD pid;
    yhook_ipc_type type;
    yhook_ipc_spawn spawn;
    yhook_ipc_hook hook;
    template <typename Writer> void Serialize(Writer &writer) const
    {
        char asctime[15];
        snprintf(asctime, 15, "%02d:%02d:%02d.%03d", time.wHour, time.wMinute,
                 time.wSecond, time.wMilliseconds);
        writer.StartObject();
        writer.String("pid");
        writer.Uint(pid);
        writer.String("timestamp");
        writer.Uint64(timestamp);
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
};

struct detour_startup
{
    std::string name;
    std::string command;
};

std::vector<file_entry> get_files();

bool detour_program(const detour_startup& startup, instance_entry& ret);
void push_message(const message_entry& e);

struct yprocmon_state
{
    unsigned short port;
    unsigned short ws_port;
    std::mutex state_mutex;
    std::vector<message_entry> messages;
    std::map<DWORD, instance_entry> instances;
} state;
