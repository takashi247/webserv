#include "socket.hpp"

#include <arpa/inet.h>  //htonlとか
#include <fcntl.h>
#include <netdb.h>   //gethostbyname
#include <unistd.h>  //close

#include <cstring>   //memset
#include <iostream>  //cout

void SetNonBlocking(int fd) {
  if (-1 == fcntl(fd, F_SETFL, O_NONBLOCK)) {
    std::cout << "Failure to set NonBlocking" << std::endl;
  }
}

void ServerSocket::SetListenfd() {
  this->listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listenfd_ == -1) {
    std::cout << "socket() failed." << std::endl;
    exit(1);
  }
  SetNonBlocking(this->listenfd_);
}

void ServerSocket::SetSockaddrIn() {
  memset(&this->serv_addr_, 0, sizeof(this->serv_addr_));
  this->serv_addr_.sin_family = AF_INET;
  if (1) {
    this->serv_addr_.sin_addr.s_addr = inet_addr(host_.c_str());
    if (serv_addr_.sin_addr.s_addr == 0xffffffff) {
      struct hostent *host;
      host = gethostbyname(host_.c_str());
      if (host == NULL) {
        return;
      }
      serv_addr_.sin_addr.s_addr = *(unsigned int *)host->h_addr_list[0];
    }
  } else {
    this->serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  this->serv_addr_.sin_port = htons(this->port_);

  std::cout << "result of inet_ntoa -> " << inet_ntoa(serv_addr_.sin_addr)
            << std::endl;
}

int ServerSocket::SetSocket() {
  ServerSocket::SetListenfd();
  int optval = 1;
  if (setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) == -1) {
    std::cout << "setsockopt() failed." << std::endl;
    close(listenfd_);
    return -1;
  }

  ServerSocket::SetSockaddrIn();
  if (bind(this->listenfd_, (struct sockaddr *)&this->serv_addr_,
           sizeof(this->serv_addr_)) == -1) {
    std::cout << "bind() failed.(" << errno << ")" << std::endl;
    close(this->listenfd_);
    return -1;
  }

  if (listen(this->listenfd_, SOMAXCONN) == -1) {
    std::cout << "listen() failed." << std::endl;
    close(this->listenfd_);
    return -1;
  }

  return 0;
}
