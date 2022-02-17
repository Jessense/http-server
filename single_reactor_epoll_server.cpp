#include "lib/TcpServer.h"

int main(int argc, char const *argv[])
{
    TcpServer tcpServer(43211);
    tcpServer.start();
    return 0;
}
