#include "TcpServer.h"
#include "Channel.h"
#include "HttpServer.h"

#include <netinet/in.h> /* sockaddr_in */
#include <unistd.h> /* read write */
#include <fcntl.h>
#include <iostream>

TcpServer::TcpServer(int port_, MessageCallback messageCallback_,int threadNum_)
    : port(port_),
      messageCallback(messageCallback_),
      eventLoop(new EventLoop()),
      acceptor(new Acceptor(port_)),
      threadNum(threadNum_),
      threadPool(new ThreadPool(eventLoop, threadNum))
{
}


TcpServer::~TcpServer()
{
}



TcpConnection* TcpServer::createConnection(int connectFd, EventLoop* subLoop)
{
    std::cout << subLoop->getTid() << " : " << connectFd << " : new tcp connection" << std::endl;
    TcpConnection *tcpConnection = new TcpConnection(connectFd, subLoop, this->messageCallback);
    return tcpConnection;
}

void TcpServer::start()
{
    threadPool->start();

    std::cout << "listen fd = " << acceptor->listen_fd << std::endl;
    Channel* channel = new Channel(acceptor->listen_fd, EPOLLIN, eventLoop, handleConnectionEstablished, NULL, this);
    eventLoop->loop();
}


int handleConnectionEstablished(void* data)
{
    TcpServer* tcpServer = (TcpServer*) data;
    Acceptor* acceptor = tcpServer->acceptor;
    int listen_fd = acceptor->listen_fd;

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connect_fd = accept(listen_fd, (sockaddr*) &client_addr, &client_len); //TODO:这三行可以封装到 Acceptor
    fcntl(connect_fd, F_SETFL, O_NONBLOCK);

    EventLoop* subLoop = tcpServer->threadPool->getNextLoop();
    
    TcpConnection* tcpConnection = tcpServer->createConnection(connect_fd, subLoop);
    
    
    return 0;
}

int handleConnectionClosed(void* data)
{
    Channel* channel = (Channel*)data;
    channel->ownerLoop->removeChannel(channel);
    return 0;
}