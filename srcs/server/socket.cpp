#include "socket.hpp"

#include <arpa/inet.h>  //htonlとか
#include <string.h>     //memset
#include <unistd.h>     //close

#include <iostream>  //cout

void Socket::set_listenfd() {
  this->listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listenfd == -1) {
    std::cout << "socket() failed." << std::endl;
    exit(1);
  }
}

void Socket::set_sockaddr_in() {
  memset(&this->serv_addr, 0, sizeof(this->serv_addr));

  this->serv_addr.sin_family = AF_INET;
  this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  this->serv_addr.sin_port = htons(this->port);
}

int Socket::set_socket() {
  Socket::set_listenfd();
  int optval = 1;
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
      -1) {
    std::cout << "setsockopt() failed." << std::endl;
    close(listenfd);
    return -1;
  }

  Socket::set_sockaddr_in();
  if (bind(this->listenfd, (struct sockaddr*)&this->serv_addr,
           sizeof(this->serv_addr)) == -1) {
    std::cout << "bind() failed.(" << errno << ")" << std::endl;
    close(this->listenfd);
    return -1;
  }

  if (listen(this->listenfd, SOMAXCONN) == -1) {
    std::cout << "listen() failed." << std::endl;
    close(this->listenfd);
    return -1;
  }

  return 0;
}
