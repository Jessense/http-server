#include "TcpConnection.h"
#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>

TcpConnection::TcpConnection(int connectFd_, EventLoop* eventLoop_, MessageCallback messageCallback_, TcpServer* tcpServer_)
    : eventLoop(eventLoop_), 
      messageCallback(messageCallback_),
      inputBuffer(new Buffer()),
      outputBuffer(new Buffer()),
      channel(new Channel(connectFd_, EPOLLIN, eventLoop, handleRead, handleWrite, handleClose, this)),
      tcpServer(tcpServer_)
{
}


TcpConnection::~TcpConnection()
{
    int fd = channel->fd;
    delete channel;
    channel = nullptr;
    int err = close(fd);
    // if (err == 0) {
    //     std::cout << "closed " << fd << std::endl;
    // } else {
    //     std::cout << "failed to close " << fd << ", err=" << err << std::endl;
    // }
    
    delete inputBuffer;
    inputBuffer = nullptr;
    delete outputBuffer;
    outputBuffer = nullptr;
    
    
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

    char *additionalBuffer = new char[Buffer::kInitBufferSize];
    
    while (true)
    {
        iovec vec[2];
        int writeable = inputBuffer->writeableSize();
        vec[0].iov_base = inputBuffer->data + inputBuffer->writeIndex;
        vec[0].iov_len = writeable;
        vec[1].iov_base = additionalBuffer;
        vec[1].iov_len = sizeof(additionalBuffer);
        int nread = readv(channel->fd, vec, 2);
        // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
        //             << "nread=" << nread << std::endl;         
        if (nread == 0) // peer FIN
        {
            // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
            //             << "FIN" << std::endl;        
            delete tcpConnection;
            tcpConnection = nullptr;
            break;
        }
        else if (nread < 0)
        {
            if (errno == EWOULDBLOCK) { // 读完了
                if (tcpConnection->messageCallback != NULL)
                {
                    auto future = tcpConnection->tcpServer->workerThreadPool->submit(tcpConnection->messageCallback, tcpConnection);
                    future.get(); // TO REMOVE
                    inputBuffer->readIndex = inputBuffer->writeIndex;
                }                
                break;
            } else {
                std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
                            << "read error" << std::endl;   
                delete tcpConnection;
                tcpConnection = nullptr;
                break; 
            }
        }
        else if (nread <= writeable)
        {
            inputBuffer->writeIndex += nread;
        }
        else
        {
            inputBuffer->writeIndex = inputBuffer->totalSize;
            inputBuffer->append(additionalBuffer, nread-writeable);
        }    
    }
    delete[] additionalBuffer;
    additionalBuffer = nullptr;
    return 0;
}

int handleWrite(void *data)
{
    TcpConnection* tcpConnection = (TcpConnection*)data;
    if (tcpConnection == nullptr) {
        return 0;
    }    
    Buffer* outputBuffer = tcpConnection->outputBuffer;
    Channel* channel = tcpConnection->channel;

    int nwrote = write(channel->fd, outputBuffer->dataBegin(), outputBuffer->readableSize());
    // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
    //             << "nwrote=" << nwrote << std::endl;      
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
                delete tcpConnection;
                tcpConnection = nullptr;
            }
        }
        // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
        //     << "handling write, nwrote=" << nwrote  << std::endl;        

    }
    // std::cout << tcpConnection->eventLoop->getTid() << " : " << tcpConnection->channel->fd << " : " 
    //     << "handling write, nwrote=" << nwrote  << std::endl;
    return nwrote;    

}

int handleClose(void *data)
{
    TcpConnection* tcpConnection = (TcpConnection*)data;
    // tcpConnection->channel->data = nullptr;
    delete tcpConnection;
    tcpConnection = nullptr;
    return 0;
}



