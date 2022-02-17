#include "TcpServer.h"
#include "Channel.h"

#include <netinet/in.h> /* sockaddr_in */
#include <unistd.h> /* read write */
#include <fcntl.h>
#include <iostream>

TcpServer::TcpServer(int port_)
    : port(port_),
      eventLoop(new EventLoop()),
      acceptor(new Acceptor(port_))
{
}

TcpServer::~TcpServer()
{
}

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

int handleRead(void* data)
{
    int connect_fd = *((int*)data);
    std::cout << "message from " << connect_fd << std::endl;
    int n;
    while (1)
    {
        char buf[512];
        if (n = read(connect_fd, buf, sizeof(buf)) > 0) 
        {
            std::cout << "n=" << n << std::endl;
            for (int i = 0; i < n; i++) 
            {
                buf[i] = rot13_char(buf[i]);
            }
            if (write(connect_fd, buf, n) < 0) {
                std::cout << "write error" << std::endl;
            }
        }
        else if (n == 0)
        {
            std::cout << "read finished" << std::endl;
            break;
        } 
        else //TODO: 处理读完/出错 连接关闭
        {
            std::cout << "read error" << std::endl;
            break;
        }        
    }
    return 0;
}

int handleConnectionEstablished(void* data)
{
    TcpServer* tcpServer = (TcpServer*) data;
    Acceptor* acceptor = tcpServer->acceptor;
    int listen_fd = acceptor->listen_fd;

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connect_fd = accept(listen_fd, (sockaddr*) &client_addr, &client_len); //TODO:这三行可以封装到 Acceptor
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);
    Channel* channel = new Channel(connect_fd, tcpServer->eventLoop);
    int* connfd = new int;
    *connfd = connect_fd;
    channel->setReadCallback(handleRead, connfd);
    
    std::cout << "new connection: " << connect_fd << std::endl;

    return 0;
}

void TcpServer::start()
{
    std::cout << "listen fd = " << acceptor->listen_fd << std::endl;
    Channel* channel = new Channel(acceptor->listen_fd, eventLoop);
    channel->setReadCallback(handleConnectionEstablished, this);
    eventLoop->loop();
}