#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "EventLoop.h"
#include "Buffer.h"

class TcpConnection;

typedef int (*MessageCallback)(TcpConnection* tcpConnection);

class TcpConnection
{
public:
    EventLoop* eventLoop;
    Channel* channel;
    Buffer* inputBuffer;
    Buffer* outputBuffer;
    MessageCallback messageCallback;
    void* data;

public:
    TcpConnection(int connectFd, EventLoop* eventLoop, MessageCallback messageCallback);
    ~TcpConnection();
    int send();
    void close();
};




#endif