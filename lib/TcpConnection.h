#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "EventLoop.h"
#include "Buffer.h"

class TcpServer;
class TcpConnection;

typedef int (*MessageCallback)(TcpConnection* tcpConnection);

class TcpConnection
{
public:
    EventLoop* eventLoop;
    Buffer* inputBuffer;
    Buffer* outputBuffer;
    MessageCallback messageCallback;
    Channel* channel;


public:
    TcpConnection(int connectFd, EventLoop* eventLoop, MessageCallback messageCallback);
    TcpConnection(){};
    virtual ~TcpConnection();
    int send();
};

int handleRead(void *data);
int handleWrite(void *data);
int handleClose(void *data);

#endif