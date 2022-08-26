#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <netinet/in.h>
#include <unistd.h>

#include "config.hpp"

extern void SetNonBlocking(int fd);

class ServerSocket {
 public:
  int listenfd_;
  size_t port_;
  in_addr_t host_;
  struct sockaddr_in serv_addr_;

 public:
  ServerSocket(int port, in_addr_t host);
  ServerSocket(const ServerSocket &other);
  ~ServerSocket();
  ServerSocket &operator=(const ServerSocket &other);

 private:
  int SetSocket();
  void SetListenfd();
  void SetSockaddrIn();

 public:
  int GetListenFd() { return this->listenfd_; }
  void Init();
};

#endif
