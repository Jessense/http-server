#include "lib/HttpServer.h"
#include <iostream>

int onRequest(HttpRequest* request, HttpResponse* response)
{
    std::string path = request->url;
    if (path == "/")
    {
        response->statusCode = OK;
        response->statusMessage = "OK";
        response->contentType = "text/html";
        response->body = "<html><head><title>This is homepage</title></head><body><h1>This is homepage</h1></body></html>";
    }
    else if (path == "/hello")
    {
        response->statusCode = OK;
        response->statusMessage = "OK";
        response->contentType = "text/plain";        
        response->body = "Hello World!";
    }
    else
    {
        response->statusCode = NotFound;
        response->statusMessage = "NotFound";
        response->keepConnected = true;
    }
    return 0;
    
}

int main(int argc, char const *argv[])
{
    HttpServer* httpServer = new HttpServer(43211, onRequest, 5);
    httpServer->start();
    return 0;
}
