#include "HttpServer.h"
#include "HttpConnection.h"
#include "utils.h"

#include <iostream>
#include <assert.h>
HttpServer::HttpServer(int port, RequestCallback requestCallback_, int threadNum)
    : TcpServer(port, onMessage, threadNum),
      requestCallback(requestCallback_)
{

}

HttpServer::~HttpServer()
{
    // std::cout << "~HttpServer()" << std::endl;
}
      
TcpConnection* HttpServer::createConnection(int connectFd, EventLoop* subLoop)
{
    // std::cout << subLoop->getTid() << " : " << connectFd << " : new http connection" << std::endl;
    HttpConnection* httpConnection = new HttpConnection(connectFd, subLoop, onMessage, this);
    return httpConnection;
}

int onMessage(TcpConnection* tcpConnection)
{
    assert(tcpConnection->eventLoop->getTid() == std::this_thread::get_id());
    HttpConnection* httpConnection = static_cast<HttpConnection*>(tcpConnection);
    // showBuffer(httpConnection->inputBuffer, tcpConnection->channel->fd);
    httpConnection->decodeRequest();
    httpConnection->httpResponse->keepAlive = httpConnection->httpRequest->keepAlive();
    
    if (httpConnection->httpRequest->currentState != REQUEST_DONE) {
        delete httpConnection;
        httpConnection = nullptr;
        return 0;
    }
    
    // std::cout << httpConnection->eventLoop->getTid() << " : " << httpConnection->channel->fd << " : " << httpConnection->httpRequest->method << " " << httpConnection->httpRequest->url << std::endl;    
    httpConnection->httpServer->requestCallback(httpConnection->httpRequest, httpConnection->httpResponse);
    httpConnection->encodeResponse();
    httpConnection->send();
    httpConnection->httpRequest->reset(); // reset for next request under keep alive
    httpConnection->httpResponse->reset();
    
    if (!httpConnection->httpResponse->keepAlive) {
        delete httpConnection; //当 request header 中有 Connnection:close 时关闭
        httpConnection = nullptr;
    }
        

    return 0;
}