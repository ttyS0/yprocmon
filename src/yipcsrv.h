/**
 * Copyright (c) 2021 sigeryeung
 *
 * @file yipcsrv.h
 * @author Siger Yang (sigeryeung@gmail.com)
 * @date 2021-09-12
 *
 * @brief IPC 服务器
 */

#pragma once

#include "debug.h"
#include "yipc.h"
#include "yprocmon.h"
#include <windows.h>

#define CONNECTING_STATE 0
#define READING_STATE 1
#define WRITING_STATE 2
#define INSTANCES 4
#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct
{
    OVERLAPPED oOverlap;
    HANDLE hPipeInst;
    CHAR chRequest[BUFSIZE];
    DWORD cbRead;
    CHAR chReply[BUFSIZE];
    DWORD cbToWrite;
    DWORD dwState;
    BOOL fPendingIO;
} PIPEINST, *LPPIPEINST;

VOID DisconnectAndReconnect(DWORD);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);

PIPEINST Pipe[INSTANCES];
HANDLE hEvents[INSTANCES];

int start_ipc_server()
{
    // yprocmon_state *state = (yprocmon_state *)param;
    DWORD i, dwWait, cbRet, dwErr;
    BOOL fSuccess;
    LPTSTR lpszPipename = TEXT(IPC_PIPE);

    // The initial loop creates several instances of a named pipe
    // along with an event object for each instance.  An
    // overlapped ConnectNamedPipe operation is started for
    // each instance.

    for (i = 0; i < INSTANCES; i++)
    {

        // Create an event object for this instance.

        hEvents[i] = CreateEvent(NULL,  // default security attribute
                                 TRUE,  // manual-reset event
                                 TRUE,  // initial state = signaled
                                 NULL); // unnamed event object

        if (hEvents[i] == NULL)
        {
            console_print("CreateEvent failed with %d.\n", GetLastError());
            return 0;
        }

        Pipe[i].oOverlap.hEvent = hEvents[i];

        Pipe[i].hPipeInst =
            CreateNamedPipe(lpszPipename,               // pipe name
                            PIPE_ACCESS_DUPLEX |        // read/write access
                                FILE_FLAG_OVERLAPPED,   // overlapped mode
                            PIPE_TYPE_MESSAGE |         // message-type pipe
                                PIPE_READMODE_MESSAGE | // message-read mode
                                PIPE_WAIT,              // blocking mode
                            INSTANCES,                  // number of instances
                            BUFSIZE * sizeof(TCHAR),    // output buffer size
                            BUFSIZE * sizeof(TCHAR),    // input buffer size
                            PIPE_TIMEOUT,               // client time-out
                            NULL); // default security attributes

        if (Pipe[i].hPipeInst == INVALID_HANDLE_VALUE)
        {
            console_print("CreateNamedPipe failed with %d.\n", GetLastError());
            return 0;
        }

        // Call the subroutine to connect to the new client

        Pipe[i].fPendingIO =
            ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);

        Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE
                                             : // still connecting
                              READING_STATE;   // ready to read
    }

    while (1)
    {
        // Wait for the event object to be signaled, indicating
        // completion of an overlapped read, write, or
        // connect operation.

        dwWait = WaitForMultipleObjects(INSTANCES, // number of event objects
                                        hEvents,   // array of event objects
                                        FALSE,     // does not wait for all
                                        INFINITE); // waits indefinitely

        // dwWait shows which pipe completed the operation.

        i = dwWait - WAIT_OBJECT_0; // determines which pipe
        if (i < 0 || i > (INSTANCES - 1))
        {
            console_print("Index out of range.\n");
            return 0;
        }

        // Get the result if the operation was pending.

        if (Pipe[i].fPendingIO)
        {
            fSuccess =
                GetOverlappedResult(Pipe[i].hPipeInst, // handle to pipe
                                    &Pipe[i].oOverlap, // OVERLAPPED structure
                                    &cbRet,            // bytes transferred
                                    FALSE);            // do not wait

            switch (Pipe[i].dwState)
            {
                // Pending connect operation
            case CONNECTING_STATE:
                if (!fSuccess)
                {
                    console_print("Error %d.\n", GetLastError());
                    return 0;
                }
                Pipe[i].dwState = READING_STATE;
                break;

                // Pending read operation
            case READING_STATE:
                if (!fSuccess || cbRet == 0)
                {
                    DisconnectAndReconnect(i);
                    continue;
                }
                Pipe[i].cbRead = cbRet;
                Pipe[i].dwState = WRITING_STATE;
                break;

                // Pending write operation
            case WRITING_STATE:
                if (!fSuccess || cbRet != Pipe[i].cbToWrite)
                {
                    DisconnectAndReconnect(i);
                    continue;
                }
                Pipe[i].dwState = READING_STATE;
                break;

            default:
            {
                console_print("Invalid pipe state.\n");
                return 0;
            }
            }
        }

        // The pipe state determines which operation to do next.

        switch (Pipe[i].dwState)
        {
            // READING_STATE:
            // The pipe instance is connected to the client
            // and is ready to read a request from the client.

        case READING_STATE:
            fSuccess = ReadFile(Pipe[i].hPipeInst, Pipe[i].chRequest,
                                BUFSIZE * sizeof(TCHAR), &Pipe[i].cbRead,
                                &Pipe[i].oOverlap);

            // The read operation completed successfully.

            if (fSuccess && Pipe[i].cbRead != 0)
            {
                Pipe[i].fPendingIO = FALSE;
                Pipe[i].dwState = WRITING_STATE;
                continue;
            }

            // The read operation is still pending.

            dwErr = GetLastError();
            if (!fSuccess && (dwErr == ERROR_IO_PENDING))
            {
                Pipe[i].fPendingIO = TRUE;
                continue;
            }

            // An error occurred; disconnect from the client.

            DisconnectAndReconnect(i);
            break;

            // WRITING_STATE:
            // The request was successfully read from the client.
            // Get the reply data and write it to the client.

        case WRITING_STATE:
            GetAnswerToRequest(&Pipe[i]);

            fSuccess = WriteFile(Pipe[i].hPipeInst, Pipe[i].chReply,
                                 Pipe[i].cbToWrite, &cbRet, &Pipe[i].oOverlap);

            // The write operation completed successfully.

            if (fSuccess && cbRet == Pipe[i].cbToWrite)
            {
                Pipe[i].fPendingIO = FALSE;
                Pipe[i].dwState = READING_STATE;
                continue;
            }

            // The write operation is still pending.

            dwErr = GetLastError();
            if (!fSuccess && (dwErr == ERROR_IO_PENDING))
            {
                Pipe[i].fPendingIO = TRUE;
                continue;
            }

            // An error occurred; disconnect from the client.

            DisconnectAndReconnect(i);
            break;

        default:
        {
            console_print("Invalid pipe state.\n");
            return 0;
        }
        }
    }

    return 0;
}

// DisconnectAndReconnect(DWORD)
// This function is called when an error occurs or when the client
// closes its handle to the pipe. Disconnect from this client, then
// call ConnectNamedPipe to wait for another client to connect.

VOID DisconnectAndReconnect(DWORD i)
{
    // Disconnect the pipe instance.

    if (!DisconnectNamedPipe(Pipe[i].hPipeInst))
    {
        console_print("DisconnectNamedPipe failed with %d.\n", GetLastError());
    }

    // Call a subroutine to connect to the new client.

    Pipe[i].fPendingIO =
        ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);

    Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE
                                         : // still connecting
                          READING_STATE;   // ready to read
}

// ConnectToNewClient(HANDLE, LPOVERLAPPED)
// This function is called to start an overlapped connect operation.
// It returns TRUE if an operation is pending or FALSE if the
// connection has been completed.

BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
    BOOL fConnected, fPendingIO = FALSE;

    // Start an overlapped connection for this pipe instance.
    fConnected = ConnectNamedPipe(hPipe, lpo);

    // Overlapped ConnectNamedPipe should return zero.
    if (fConnected)
    {
        console_print("ConnectNamedPipe failed with %d.\n", GetLastError());
        return 0;
    }

    switch (GetLastError())
    {
        // The overlapped connection in progress.
    case ERROR_IO_PENDING:
        fPendingIO = TRUE;
        break;

        // Client is already connected, so signal an event.

    case ERROR_PIPE_CONNECTED:
        if (SetEvent(lpo->hEvent))
            break;

        // If an error occurs during the connect operation...
    default:
    {
        console_print("ConnectNamedPipe failed with %d.\n", GetLastError());
        return 0;
    }
    }

    return fPendingIO;
}

VOID GetAnswerToRequest(LPPIPEINST pipe)
{
    // _tconsole_print(TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
    // StringCchCopy(pipe->chReply, BUFSIZE, TEXT("Default answer from
    // server")); pipe->cbToWrite = (lstrlen(pipe->chReply) + 1) *
    // sizeof(TCHAR); console_print("%s", pipe->chRequest); std::string
    // request(pipe->chRequest, BUFSIZE);
    size_t processed = 0;
    unsigned char *p = (unsigned char *)pipe->chRequest;
    while (processed < pipe->cbRead)
    {
        yhook_message *m = (yhook_message *)p;
        yhook_message_entry e;
        std::string message(m->message, m->length);
        // console_print(
        //     "[IPC] Receive IPC message from, time: %02d:%02d:%02d.%d, "
        //     "type: %d, length: %u\n",
        //     m->time.wHour, m->time.wMinute, m->time.wSecond,
        //     m->time.wMilliseconds, m->type, m->length);
        // for (size_t i = 0; i < 40; i++)
        // {
        //     console_print("%02X ", (unsigned char)pipe->chRequest[i]);
        // }
        // console_print("\n");
        e.type = m->type;
        e.timestamp = m->timestamp;
        memcpy(&e.time, &m->time, sizeof(e.time));
        if (m->type == YHOOK_IPC_SPAWN)
        {
            const yhook_ipc_spawn &spawn = parse_spawn_message(message);
            // console_print("\t[IPC][SPAWN] Name: %s PID: %u\n",
            //               spawn.process.c_str(), spawn.pid);
            e.spawn = spawn;
        }
        else if (m->type == YHOOK_IPC_HOOK)
        {
            const yhook_ipc_hook &hook = parse_hook_message(message);
            // console_print("\t[IPC][HOOK] Name: %s\n", hook.name.c_str());
            // size_t i = 0;
            // for (auto const &arg : hook.args)
            // {
            //     console_print("\t[IPC][HOOK] Arg [%d] %s: %s\n", i,
            //                   arg.first.c_str(), arg.second.c_str());
            //     i++;
            // }
            e.hook = hook;
        }
        // console_print("\n");
        std::lock_guard<std::mutex> guard(state.operations_mutex);
        state.operations.push_back(e);
        p += m->length;
        processed += m->length;
        break;
    }
}
