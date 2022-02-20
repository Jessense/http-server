#include "TcpServer.h"
#include "Channel.h"

#include <netinet/in.h> /* sockaddr_in */
#include <unistd.h> /* read write */
#include <fcntl.h>
#include <iostream>

TcpServer::TcpServer(int port_, int threadNum_)
    : port(port_),
      eventLoop(new EventLoop()),
      acceptor(new Acceptor(port_)),
      threadNum(threadNum_),
      threadPool(new ThreadPool(eventLoop, threadNum))
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

int handleConnectionClosed(void* data)
{
    Channel* channel = (Channel*)data;
    channel->ownerLoop->removeChannel(channel);
    return 0;
}

int handleRead(void* data)
{
    Channel* channel = (Channel*)data;
    int connect_fd = channel->fd;
    int n;
    while (true)
    {
        char buf[512];
        if ((n = read(connect_fd, buf, 512)) > 0) 
        {
            std::string s = "";
            for (int i = 0; i < n-1; i++) {
                s += buf[i];
            }
            std::cout << std::this_thread::get_id() << " : " << connect_fd << " : " << s << std::endl;
            for (int i = 0; i < n; i++) 
            {
                buf[i] = rot13_char(buf[i]);
            }
            if (write(connect_fd, buf, n) < 0) {
                std::cout << std::this_thread::get_id() << " : " << connect_fd << " : write error" << std::endl;
            }
            
        }
        else if (n == 0) //对端发送了 FIN 要关闭连接
        {
            std::cout << std::this_thread::get_id() << " : " << connect_fd << " : FIN" << std::endl;
            handleConnectionClosed(channel);
            break;
        } 
        else //TODO: 处理读完/出错 连接关闭
        {
            if (errno != EAGAIN) // EAGAIN 属于非阻塞 read 的正常结束？
                std::cout << std::this_thread::get_id() << " : " << connect_fd << " : read error" << std::endl;
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
    fcntl(connect_fd, F_SETFL, O_NONBLOCK);

    EventLoop* subLoop = tcpServer->threadPool->getNextLoop();
    Channel* channel = new Channel(connect_fd, subLoop);
    channel->setReadCallback(handleRead, channel);
    
    std::cout << subLoop->getTid() << " : " << connect_fd << " : new connection" << std::endl;

    return 0;
}

void TcpServer::start()
{
    threadPool->start();

    std::cout << "listen fd = " << acceptor->listen_fd << std::endl;
    Channel* channel = new Channel(acceptor->listen_fd, eventLoop);
    channel->setReadCallback(handleConnectionEstablished, this);
    eventLoop->loop();
}