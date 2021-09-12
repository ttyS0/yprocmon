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
#include <mutex>
#include <vector>
#include <windows.h>

#define QUOTE(x) #x
#define QUOTE_EXPANDED(x) QUOTE(x)
#define YPROCMON_HTTP_PORT 8051
#define YPROCMON_HTTP_URL "http://localhost:" QUOTE_EXPANDED(YPROCMON_HTTP_PORT)

struct yprocmon_state
{
    unsigned short port;
    std::mutex operations_mutex;
    std::vector<yhook_message_entry> operations;
} state;
