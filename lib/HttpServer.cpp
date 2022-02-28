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
      
TcpConnection* HttpServer::createConnection(int connectFd, EventLoop* subLoop)
{
    HttpConnection* httpConnection = new HttpConnection(connectFd, subLoop, onMessage, this);
    return httpConnection;
}

int onMessage(TcpConnection* tcpConnection)
{
    assert(tcpConnection->eventLoop->getTid() == std::this_thread::get_id());
    HttpConnection* httpConnection = static_cast<HttpConnection*>(tcpConnection);
    httpConnection->decodeRequest();
    if (httpConnection->httpRequest->currentState != REQUEST_DONE)
        return 0;
    httpConnection->httpServer->requestCallback(httpConnection->httpRequest, httpConnection->httpResponse);
    httpConnection->encodeResponse();
    httpConnection->send();
    // httpConnection->close(); //TODO: 处理连接关闭，当 request header 中有 Connnection:close 时关闭
    return 0;
}