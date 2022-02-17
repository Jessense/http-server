#ifndef ACCEPTOR_H
#define ACCEPTOR_H

class Acceptor
{
public:
    int listen_port;
    int listen_fd;
public:
    static const int kListenMax = 1024;
    Acceptor(int port);
    ~Acceptor();
};


#endif