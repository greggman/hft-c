//
//  websocket.cpp
//  hft-sdl
//
//  Created by GREGG TAVARES on 5/20/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#include "websocket.h"
#include "easywsclient.hpp"
#include <string>
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif


//#include "easywsclient.cpp" // <-- include only if you don't want compile separately

using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

void handle_message(const std::string & message)
{
    printf(">>> %s\n", message.c_str());
    if (message == "world") { ws->close(); }
}


int foobarmain()
{
#ifdef _WIN32
    INT rc;
    WSADATA wsaData;
    
    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        printf("WSAStartup Failed.\n");
        return 1;
    }
#endif
    
    ws = WebSocket::from_url("ws://localhost:18679/");
    if (!ws) {
        printf("ERROR: no connection\n");
    } else {
        printf("start:");
        while (ws->getReadyState() != WebSocket::CLOSED) {
            ws->poll();
            ws->send("hello");
            ws->dispatch(handle_message);
            // ...do more stuff...
        }
        delete ws; // alternatively, use unique_ptr<> if you have C++11
    }
#ifdef _WIN32
    WSACleanup();
#endif
    
    return 0;
}