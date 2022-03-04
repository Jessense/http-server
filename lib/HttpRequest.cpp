#include "HttpRequest.h"

void HttpRequest::reset()
{
    version.clear();
    method.clear();
    url.clear();
    currentState = REQUEST_STATUS;
    requestHeaders.clear();
}

bool HttpRequest::keepAlive()
{
    if (requestHeaders.find("Connection") != requestHeaders.end() && requestHeaders["Connection"] == "close") {
        return false;
    }
    return true;
}