#include "lib/TcpServer.h"
#include "lib/utils.h"

#include <iostream>

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}



int onMessage(TcpConnection* tcpConnection)
{
    Buffer* inputBuffer = tcpConnection->inputBuffer;
    showBuffer(inputBuffer, tcpConnection->channel->fd);
    Buffer* outputBuffer = tcpConnection->outputBuffer;
    for (int i = inputBuffer->readIndex; i < inputBuffer->writeIndex; i++)
    {
        outputBuffer->appendChar(rot13_char(inputBuffer->data[i]));
    }
    showBuffer(outputBuffer, tcpConnection->channel->fd);
    tcpConnection->send();
    return 0;
}

int main(int argc, char const *argv[])
{
    TcpServer tcpServer(43211, onMessage, 5);
    tcpServer.start();
    return 0;
}
