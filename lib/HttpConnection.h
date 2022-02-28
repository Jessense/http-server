#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpConnection : public TcpConnection
{
public:
    HttpServer* httpServer;
    HttpRequest* httpRequest;
    HttpResponse* httpResponse;
public:
    HttpConnection(int connectFd, EventLoop* eventLoop, MessageCallback messageCallback, HttpServer* httpServer_);
    ~HttpConnection();
    int decodeRequest();
    int encodeResponse();
};




#endif