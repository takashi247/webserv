#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <cerrno>
#include <cstdlib>

#include "config.hpp"

class Socket {
 public:
  int listenfd_;
  int port_;
  struct sockaddr_in serv_addr_;
  const ServerConfig* p_sc_;

 public:
  explicit Socket(int port, const ServerConfig* config)
      : port_(port), p_sc_(config) {
    SetSocket();
  }
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
  int listenfd_;
  int fd_;
  const ServerConfig* p_sc_;
  ClientSocket(int fd, const ServerConfig* conf) : fd_(fd), p_sc_(conf) {}
};
#endif
