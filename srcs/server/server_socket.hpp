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
  std::string host_;
  struct sockaddr_in serv_addr_;

 public:
  ServerSocket(int port, std::string host);
  ServerSocket(const ServerSocket &other);
  ~ServerSocket() { close(listenfd_); }
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
