#ifndef _SOCHET_H_
#define _SOCHET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "configure.h"

class Socket {
    int listenfd;
    int port;
    struct sockaddr_in serv_addr;
public:
    explicit Socket(int port_): port{port_} {}
    void set_listenfd();
    void set_sockaddr_in();
    int set_socket();

    int get_listenfd() const { return this->listenfd; }
};

#endif