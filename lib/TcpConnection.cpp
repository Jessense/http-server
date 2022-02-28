#include "TcpConnection.h"
#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

TcpConnection::TcpConnection(int connectFd_, EventLoop* eventLoop_, MessageCallback messageCallback_)
    : eventLoop(eventLoop_), 
      messageCallback(messageCallback_),
      inputBuffer(new Buffer()),
      outputBuffer(new Buffer()),
      channel(new Channel(connectFd_, EPOLLIN, eventLoop, handleRead, handleWrite, this))
{
    std::cout << "inputBuffer=" << inputBuffer <<std::endl;
    std::cout << "outputBuffer=" << outputBuffer <<std::endl;
}


TcpConnection::~TcpConnection()
{
}

int TcpConnection::send()
{
    return handleWrite(this);
}

int TcpConnection::close()
{
    channel->deregister();
    return shutdown(channel->fd, SHUT_WR);
}


int handleRead(void *data)
{
    TcpConnection* tcpConnection = static_cast<TcpConnection*>(data);
    Buffer* inputBuffer = tcpConnection->inputBuffer;
    Channel* channel = tcpConnection->channel;

    int nread = inputBuffer->readSocket(channel->fd);
    if (nread > 0)
    {
        if (tcpConnection->messageCallback != NULL)
        {
            tcpConnection->messageCallback(tcpConnection);
        }
    }
    else
    {
        tcpConnection->close();
    }

    return nread;
}

int handleWrite(void *data)
{
    TcpConnection* tcpConnection = (TcpConnection*)data;
    Buffer* outputBuffer = tcpConnection->outputBuffer;
    Channel* channel = tcpConnection->channel;

    int nwrote = write(channel->fd, outputBuffer->dataBegin(), outputBuffer->readableSize());
    if (nwrote > 0)
    {
        outputBuffer->readIndex += nwrote;
        if (outputBuffer->readableSize() == 0)
        {
            channel->disableWrite();
        } else if (!channel->writeEnabled() && outputBuffer->readableSize() > 0) //如果未发送完
        {
            channel->enableWrite();
        }
    }
    else
    {
        nwrote = 0;
        if (errno != EWOULDBLOCK) {
            if (errno == EPIPE || errno == ECONNRESET) //TODO
            {
                std::cout << "write error" << std::endl;
            }
        }
    }
    return nwrote;    

}



