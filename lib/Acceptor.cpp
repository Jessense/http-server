#include "Acceptor.h"

#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>  /* for nonblocking */
#include <netinet/in.h> /* sockaddr_in */


Acceptor::Acceptor(int port): listen_port(port)
{
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int rt1 = bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        std::cout << "bind failed" << std::endl;
    }

    int rt2 = listen(listen_fd, kListenMax);
    if (rt2 < 0) {
        std::cout << "listen failed" << std::endl;
    }

}

Acceptor::~Acceptor()
{
}