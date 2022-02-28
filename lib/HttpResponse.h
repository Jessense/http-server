#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <vector>
#include <map>


enum HttpStatusCode {
    Unknown,
    OK = 200,
    MovedPermanently = 301,
    BadRequest = 400,
    NotFound = 404,
};

class HttpResponse
{
public:
    HttpStatusCode statusCode;
    std::string statusMessage;
    std::string contentType;
    std::string body;
    std::map<std::string, std::string> responseHeaders;
    int responseHeadersNum;
    bool keepConnected;

};




#endif