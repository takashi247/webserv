#include "server_socket.hpp"

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

ServerSocket::ServerSocket(int port, in_addr_t host)
    : port_(port), host_(host) {}
ServerSocket::ServerSocket(const ServerSocket &other) { *this = other; }

ServerSocket &ServerSocket::operator=(const ServerSocket &other) {
  port_ = other.port_;
  host_ = other.host_;
  return *this;
}

void ServerSocket::Init() { SetSocket(); }
void ServerSocket::SetListenfd() {
  this->listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listenfd_ == -1) {
    throw WebservException("socket() failed.");
  }
  SetNonBlocking(this->listenfd_);
}

void ServerSocket::SetSockaddrIn() {
  memset(&this->serv_addr_, 0, sizeof(this->serv_addr_));
  this->serv_addr_.sin_family = AF_INET;
  if (1) {
    this->serv_addr_.sin_addr.s_addr = host_;
  } else {
    this->serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  }
  this->serv_addr_.sin_port = htons(this->port_);
}

int ServerSocket::SetSocket() {
  ServerSocket::SetListenfd();
  int optval = 1;
  if (setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) == -1) {
    close(listenfd_);
    throw WebservException("setsockopt() failed.");
  }

  ServerSocket::SetSockaddrIn();
  if (bind(this->listenfd_, (struct sockaddr *)&this->serv_addr_,
           sizeof(this->serv_addr_)) == -1) {
    std::ostringstream oss;
    oss << "bind() failed.(" << errno << ")";
    close(this->listenfd_);
    throw WebservException(oss.str());
  }

  if (listen(this->listenfd_, SOMAXCONN) == -1) {
    close(this->listenfd_);
    throw WebservException("listen() failed.");
  }

  return 0;
}
