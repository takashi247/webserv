#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

class Socket {
  int listenfd;
  int port;
  struct sockaddr_in serv_addr;

 public:
  explicit Socket(int port_) : port(port_) {}
  ~Socket() {}

 private:
  void SetListenfd();
  void SetSockaddrIn();

 public:
  int SetSocket();
  int GetListenFd() const { return this->listenfd; }
};

#endif
