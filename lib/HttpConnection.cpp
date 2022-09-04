#include "HttpConnection.h"

#include <iostream>

HttpConnection::HttpConnection(int connectFd_, EventLoop* eventLoop_, MessageCallback messageCallback_, HttpServer* httpServer_)
    : TcpConnection(connectFd_, eventLoop_, messageCallback_, httpServer_),
      httpServer(httpServer_),
      httpRequest(new HttpRequest()),
      httpResponse(new HttpResponse())
{

}

HttpConnection::~HttpConnection()
{
    delete httpRequest;
    delete httpResponse;
    httpRequest = nullptr;
    httpResponse = nullptr;
    // std::cout << "~HttpConnection()" << std::endl;
}



int HttpConnection::decodeRequest()
{
    int ok = 1; // TODO：错误处理
    int crlf = 0;
    bool post = false;
    std::string s(inputBuffer->dataBegin(), inputBuffer->readableSize());
    
    while (httpRequest->currentState != REQUEST_DONE)
    {
        if (httpRequest->currentState == REQUEST_STATUS)
        {
            crlf = s.find("\r\n");
            int pos1 = s.find(" ");
            httpRequest->method = s.substr(0, pos1);
            if (httpRequest->method == "POST") {
                post = true;
            }
            int pos2 = s.find(" ", pos1+1);
            httpRequest->url = s.substr(pos1+1, pos2-pos1-1);
            httpRequest->version = s.substr(pos2+1, crlf-pos2-1);
            httpRequest->currentState = REQUEST_HEADERS;
        }
        else if (httpRequest->currentState == REQUEST_HEADERS) //header中分号后要加一个空格
        {
            while (true)
            {
                int pos1 = s.find(":", crlf+2);
                if (pos1 == std::string::npos)
                    break;
                std::string key = s.substr(crlf+2, pos1-crlf-2);
                crlf = s.find("\r\n", pos1+2);
                std::string value = s.substr(pos1+2, crlf-pos1-2);
                httpRequest->requestHeaders[key] = value;
            }
            
            httpRequest->currentState = REQUEST_BODY;
        }
        else if (httpRequest->currentState == REQUEST_BODY)
        {
            if (post) {
                httpRequest->body = s.substr(crlf + 4, std::stoi(httpRequest->requestHeaders["Content-Length"]));
            }
            httpRequest->currentState = REQUEST_DONE;
        }
    }
    return ok;
}

int HttpConnection::encodeResponse()
{
    std::string statusLine = "HTTP/1.1 " + std::to_string(httpResponse->statusCode) 
        + " " + httpResponse->statusMessage + "\r\n";
    outputBuffer->appendString(statusLine);
    std::string contentLen = std::to_string(httpResponse->body.size());
    outputBuffer->appendString("Content-Length: " + contentLen + "\r\n");    
    if (!httpResponse->keepAlive)
    {
        outputBuffer->appendString("Connection: close\r\n");
    }
    else
    {
        outputBuffer->appendString("Connection: Keep-Alive\r\n");
    }

    for (auto it = httpResponse->responseHeaders.begin(); it != httpResponse->responseHeaders.end(); it++)
    {
        outputBuffer->appendString(it->first + ": " + it->second + "\r\n");
    }

    outputBuffer->appendString("\r\n");
    outputBuffer->appendString(httpResponse->body);
    return 0;
}