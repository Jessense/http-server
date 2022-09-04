#include "lib/HttpServer.h"
#include <iostream>

int onRequest(HttpRequest* request, HttpResponse* response)
{
    std::string path = request->url;
    if (path == "/")
    {
        if (request->method != "POST") {
            response->statusCode = OK;
            response->statusMessage = "OK";
            response->contentType = "text/html";
            response->body = "<html><head><title>This is homepage</title></head><body><h1>Hello World!</h1></body></html>\n";
        } else {
            response->statusCode = OK;
            response->statusMessage = "OK";
            response->contentType = "text/plain";
            response->body = request->body;
        }
    }
    else if (path == "/hello")
    {
        response->statusCode = OK;
        response->statusMessage = "OK";
        response->contentType = "text/plain";        
        response->body = "Hello World!\n";
    }
    else
    {
        response->statusCode = NotFound;
        response->statusMessage = "NotFound\n";
    }
    return 0;
    
}

int main(int argc, char const *argv[])
{
    HttpServer httpServer(43211, onRequest, 6);
    httpServer.start();
    return 0;
}
