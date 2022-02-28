#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"



typedef int (*RequestCallback)(HttpRequest *httpRequest, HttpResponse *httpResponse);


class HttpServer : public TcpServer
{
public:
    RequestCallback requestCallback;
public:
    HttpServer(int port, RequestCallback requestCallback_, int threadNum=0);
    ~HttpServer();
    TcpConnection* createConnection(int connectFd, EventLoop* subLoop) override;
};



int onMessage(TcpConnection* tcpConnection);

#endif