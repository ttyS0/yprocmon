/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yhttp.h
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-10
 *
 * @brief HTTP 服务器
 */

#pragma once

#include "debug.h"
#include "yprocmon.h"
#include <fstream>
#include <httplib.h>
#include <mutex>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

httplib::Server svr;

void start_http_server()
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
    svr.Get("/api/operations",
            [&](const httplib::Request &req, httplib::Response &res)
            {
                std::lock_guard<std::mutex> guard(state.operations_mutex);
                rapidjson::StringBuffer s;
                rapidjson::Writer<rapidjson::StringBuffer> writer(s);
                writer.StartArray();
                if (req.has_param("after"))
                {
                    time_t after = atoll(req.get_param_value("after").c_str());
                    console_print(
                        "[HTTP] Filtering operations after timestamp %lld.\n",
                        after);
                    auto start =
                        std::upper_bound(state.operations.begin(),
                                         state.operations.end(), after);
                    for (auto e = start; e != state.operations.end(); e++)
                    {
                        e->Serialize(writer);
                    }
                }
                else
                {
                    for (auto const &e : state.operations)
                    {
                        e.Serialize(writer);
                    }
                }
                writer.EndArray();
                res.set_content(s.GetString(), "application/json");
            });
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