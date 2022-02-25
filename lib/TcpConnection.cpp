#include "TcpConnection.h"

#include <iostream>
#include <unistd.h>

int handleRead(void *data);
int handleWrite(void *data);

TcpConnection::TcpConnection(int connectFd_, EventLoop* eventLoop_, MessageCallback messageCallback_)
    : eventLoop(eventLoop_), 
      channel(new Channel(connectFd_, EPOLLIN, eventLoop, handleRead, handleWrite, this)),
      messageCallback(messageCallback_),
      inputBuffer(new Buffer()),
      outputBuffer(new Buffer())
{
}

TcpConnection::~TcpConnection()
{
}

int TcpConnection::send()
{
    return handleWrite(this);
}

void TcpConnection::close()
{
    channel->deregister();
}

int handleRead(void *data)
{
    TcpConnection* tcpConnection = (TcpConnection*)data;
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



