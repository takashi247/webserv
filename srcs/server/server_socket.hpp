#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <netinet/in.h>

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
  ~ServerSocket() {}
  ServerSocket &operator=(const ServerSocket &other);

 private:
  int SetSocket();
  void SetListenfd();
  void SetSockaddrIn();

 public:
  int GetListenFd() const { return this->listenfd_; }
  void Init();
};

#endif
