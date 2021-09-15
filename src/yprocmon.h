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

enum instance_status {
    INSTANCE_SPAWNED,
    INSTANCE_EXITED
};

struct instance_entry {
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

struct detour_startup
{
    std::string name;
    std::string command;
};

std::vector<file_entry> get_files();

bool detour_program(const detour_startup& startup, instance_entry& ret);

struct yprocmon_state
{
    unsigned short port;
    std::mutex operations_mutex;
    std::mutex instances_mutex;
    std::vector<yhook_message_entry> operations;
    std::vector<instance_entry> instances;
} state;
