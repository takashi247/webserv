#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <cerrno>
#include <cstdlib>

#include "config.hpp"

class Socket {
 public:
  int listenfd_;
  size_t port_;
  struct sockaddr_in serv_addr_;

 public:
  explicit Socket(int port) : port_(port) { SetSocket(); }
  ~Socket() {}

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
  ClientSocket(int fd) : fd_(fd) {}
};
#endif
