#include "HttpResponse.h"

HttpResponse::HttpResponse()
    : statusCode(Unknown),
      statusMessage(),
      contentType(),
      body(),
      responseHeaders(),
      keepAlive(true)
{
}

void HttpResponse::reset()
{
    statusCode = Unknown;
    statusMessage.clear();
    contentType.clear();
    body.clear();
    responseHeaders.clear();
    keepAlive = true;
}