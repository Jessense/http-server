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
}


TcpConnection::~TcpConnection()
{
    channel->deregister();
    int err = close(channel->fd);
    if (err == 0) {
        std::cout << "closed " << channel->fd << std::endl;
    } else {
        std::cout << "failed to close " << channel->fd << ", err=" << err << std::endl;
    }
    delete channel;

    delete inputBuffer;
    delete outputBuffer;
    
    // shutdown(channel->fd, SHUT_WR);

    std::cout << "~TcpConnection()" << std::endl;
}

int TcpConnection::send()
{
    return handleWrite(this);
}



int handleRead(void *data)
{
    TcpConnection* tcpConnection = static_cast<TcpConnection*>(data);
    Buffer* inputBuffer = tcpConnection->inputBuffer;
    Channel* channel = tcpConnection->channel;
    // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " << "handling read" << std::endl;
    int nread = inputBuffer->readSocket(channel->fd);
    // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
    //         << "handling read, nread=" << nread << ", readIndex=" << inputBuffer->readIndex << ", writeIndex=" << inputBuffer->writeIndex << std::endl;
    if (nread > 0)
    {
        if (tcpConnection->messageCallback != NULL)
        {
            tcpConnection->messageCallback(tcpConnection);
            inputBuffer->readIndex = inputBuffer->writeIndex;
        }
    }
    else //TODO
    {
        delete tcpConnection;
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
        if (outputBuffer->readableSize() == 0 && channel->writeEnabled())
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
    // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
    //     << "handling write, nwrote=" << nwrote << "readIndex=" << outputBuffer->readIndex << ", writeIndex=" << outputBuffer->writeIndex << std::endl;
    return nwrote;    

}



