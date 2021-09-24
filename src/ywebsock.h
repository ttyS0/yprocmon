#pragma once
#define _WEBSOCKETPP_CPP11_THREAD_
#define ASIO_STANDALONE
#include "debug.h"
#include "yprocmon.h"
#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

typedef websocketpp::server<websocketpp::config::asio> ws_server;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::condition_variable;
using websocketpp::lib::lock_guard;
using websocketpp::lib::mutex;
using websocketpp::lib::thread;
using websocketpp::lib::unique_lock;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

enum action_type
{
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE
};

struct action
{
    action(action_type t, connection_hdl h) : type(t), hdl(h) {}
    action(action_type t, connection_hdl h, ws_server::message_ptr m)
        : type(t), hdl(h), msg(m)
    {
    }
    action(action_type t, connection_hdl h, std::string d)
        : type(t), hdl(h), data(d)
    {
    }
    action(action_type t, const message_entry &e) : type(t), entry(e) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    ws_server::message_ptr msg;
    std::string data;
    message_entry entry;
};

typedef ws_server::message_ptr message_ptr;

class broadcast_server
{
public:
    broadcast_server()
    {
        m_server.clear_access_channels(websocketpp::log::alevel::all);
        m_server.set_access_channels(websocketpp::log::alevel::connect);
        // Initialize Asio Transport
        m_server.init_asio();

        // Register handler callbacks
        m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
        m_server.set_close_handler(
            bind(&broadcast_server::on_close, this, ::_1));
        // m_server.set_message_handler(bind(&broadcast_server::on_message,this,::_1,::_2));
    }

    void run(uint16_t port)
    {
        // listen on specified port
        m_server.listen(port);

        // Start the server accept loop
        m_server.start_accept();

        // Start the ASIO io_service run loop
        try
        {
            m_server.run();
        }
        catch (const std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void on_open(connection_hdl hdl)
    {
        {
            lock_guard<mutex> guard(m_action_lock);
            // std::cout << "on_open" << std::endl;
            m_actions.push(action(SUBSCRIBE, hdl));
        }
        m_action_cond.notify_one();
    }

    // void on_message(connection_hdl hdl, server::message_ptr msg)
    // {
    //     // queue message up for sending by processing thread
    //     {
    //         lock_guard<mutex> guard(m_action_lock);
    //         // std::cout << "on_message" << std::endl;
    //         m_actions.push(action(MESSAGE, hdl, msg));
    //     }
    //     m_action_cond.notify_one();
    // }

    void push_message_entry(const message_entry &e)
    {
        {
            lock_guard<mutex> guard(m_action_lock);
            m_actions.push(action(MESSAGE, e));
        }
        m_action_cond.notify_one();
    }

    void on_close(connection_hdl hdl)
    {
        {
            lock_guard<mutex> guard(m_action_lock);
            // std::cout << "on_close" << std::endl;
            m_actions.push(action(UNSUBSCRIBE, hdl));
        }
        m_action_cond.notify_one();
    }

    void process_messages()
    {
        while (true)
        {
            unique_lock<mutex> lock(m_action_lock);

            while (m_actions.empty())
            {
                m_action_cond.wait(lock);
            }

            console_print("[HTTP/WS] Processing message queue (size=%d)\n",
                          m_actions.size());
            bool hasData = false;
            std::vector<message_entry> sent;
            while (!m_actions.empty())
            {
                action a = m_actions.front();
                m_actions.pop();

                if (a.type == SUBSCRIBE)
                {
                    lock_guard<mutex> guard(m_connection_lock);
                    m_connections.insert(a.hdl);
                }
                else if (a.type == UNSUBSCRIBE)
                {
                    lock_guard<mutex> guard(m_connection_lock);
                    m_connections.erase(a.hdl);
                }
                else if (a.type == MESSAGE)
                {
                    hasData = true;
                    sent.push_back(a.entry);
                    // rapidjson::Writer<rapidjson::StringBuffer,
                    // rapidjson::UTF8<>,
                    //                   rapidjson::ASCII<>>
                    //     writer(s);
                }
            }
            if (hasData)
            {
                rapidjson::StringBuffer s;
                rapidjson::Writer<rapidjson::StringBuffer> writer(s);
                writer.StartArray();
                for (auto const &e : sent)
                {
                    e.Serialize(writer);
                }
                writer.EndArray();
                const char *data = s.GetString();
                {
                    lock_guard<mutex> guard(m_connection_lock);
                    con_list::iterator it;
                    for (auto const &hdl : m_connections)
                    {
                        m_server.send(hdl, data,
                                      websocketpp::frame::opcode::binary);
                    }
                }
            }
            lock.unlock();
        }
    }

private:
    typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

    ws_server m_server;
    con_list m_connections;
    std::queue<action> m_actions;

    mutex m_action_lock;
    mutex m_connection_lock;
    condition_variable m_action_cond;
};

broadcast_server ws_broadcast;

void start_websocket_server()
{
    try
    {
        thread t(bind(&broadcast_server::process_messages, &ws_broadcast));
        ws_broadcast.run(state.ws_port);
        t.join();
    }
    catch (websocketpp::exception const &e)
    {
        std::cout << e.what() << std::endl;
    }
}