#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

#include "config.hpp"

extern void SetNonBlocking(int fd);

class ServerSocket {
 public:
  int listenfd_;
  size_t port_;
  std::string host_;
  struct sockaddr_in serv_addr_;
  sockaddr ahoge;

 public:
  explicit ServerSocket(int port, std::string host) : port_(port), host_(host) {
    SetSocket();
  }
  ~ServerSocket() {}

 private:
  void SetListenfd();
  void SetSockaddrIn();

 public:
  int SetSocket();
  int GetListenFd() const { return this->listenfd_; }
};

class ClientSocket {
 public:
  int fd_;
  const ServerSocket* parent_;
  ClientSocket(int fd, const ServerSocket* parent) : fd_(fd), parent_(parent) {
    SetNonBlocking(fd);
  }
};
#endif
