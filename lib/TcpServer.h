#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EventLoop.h"
#include "Acceptor.h"


class TcpServer
{
public:
    int port;
    EventLoop* eventLoop;
    Acceptor* acceptor;
    
public:
    TcpServer(int port);
    ~TcpServer();
    void start();

};




#endif