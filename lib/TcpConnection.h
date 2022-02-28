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
    Channel* channel;
    Buffer* inputBuffer;
    Buffer* outputBuffer;
    MessageCallback messageCallback;



public:
    TcpConnection(int connectFd, EventLoop* eventLoop, MessageCallback messageCallback);
    TcpConnection(){};
    ~TcpConnection();
    int send();
    int close();
};

int handleRead(void *data);
int handleWrite(void *data);


#endif