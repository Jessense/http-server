#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "ThreadPool.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "WorkerThreadPool.h"
#include "Logger.h"

class TcpServer
{
public:
    int port;
    EventLoop* eventLoop;
    Acceptor* acceptor;
    int threadNum;
    ThreadPool* threadPool;
    WorkerThreadPool* workerThreadPool;
    MessageCallback messageCallback;
    Logger* logger;
public:
    TcpServer(int port_, MessageCallback messageCallback_, int threadNum_=0);
    virtual ~TcpServer();
    void start();
    virtual TcpConnection* createConnection(int connectFd, EventLoop* subLoop);

};

int handleConnectionEstablished(void* data);


#endif