#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <vector>
#include <map>

enum HttpRequestState {
    REQUEST_STATUS,
    REQUEST_HEADERS,
    REQUEST_BODY,
    REQUEST_DONE
};

class HttpRequest
{
public:
    std::string version;
    std::string method;
    std::string url;
    HttpRequestState currentState;
    std::map<std::string, std::string> requestHeaders;
    std::string body;
    std::map<std::string, std::string> form;
public:
    void reset();
    bool keepAlive();
};



#endif