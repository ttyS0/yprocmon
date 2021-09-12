/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yprocmon.cc
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief yprocmon 主模块
 */

#pragma once

#include "debug.h"
#include "yprocmon.h"
#include <fstream>
#include <httplib.h>

#include <rapidjson/rapidjson.h>

httplib::Server svr;

void start_http_server(const yprocmon_state &state)
{
    svr.set_logger(
        [](const auto &req, const auto &res)
        {
            console_print("[HTTP] %s %s %s HTTP/1.1 %d\n", req.method.c_str(),
                          req.path.c_str(),
                          req.get_header_value("User-Agent").c_str(),
                          res.status);
        });
    svr.Get("/api/ping", [](const httplib::Request &, httplib::Response &res)
            { res.set_content("{\"ping\": true}", "application/json"); });
    svr.Post("/api/run",
             [](const httplib::Request &req, httplib::Response &res)
             {
                 // detour_program();
                 if (req.has_param(""))
                 {
                 }
                 res.set_content("{\"run\": true}", "application/json");
             });
    svr.Post("/api/upload",
             [&](const auto &req, auto &res)
             {
                 if (req.has_file("file"))
                 {
                     const auto &file = req.get_file_value("file");
                     // std::ofstream out()
                     std::string path("samples/");
                     path += file.filename;
                     std::ofstream out(path, std::ios::binary);
                     out << file.content;
                     out.close();
                 }
                 else
                 {
                     res.set_content("{\"error\": true}", "application/json");
                 }
             });
    svr.listen("0.0.0.0", state.port);
}