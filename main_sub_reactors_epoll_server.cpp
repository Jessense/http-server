#include "lib/TcpServer.h"

int main(int argc, char const *argv[])
{
    TcpServer tcpServer(43211, 5);
    tcpServer.start();
    return 0;
}
